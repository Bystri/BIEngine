#include "ZipFile.h"

#include <algorithm>
#include <cctype>
#include <iterator>

#include <zlib.h>

// Загрузчик ZIP-архивов. Оригинальный код пренадлежит Javier Arevalo.
// https://www.flipcode.com/archives/Zip_File_Loader.shtml

namespace BIEngine {

// Basic types
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

#pragma pack(1)

struct ZipFile::TZipLocalHeader {
   enum {
      SIGNATURE = 0x04034b50,
   };

   dword sig;
   word version;
   word flag;
   word compression; // Z_NO_COMPRESSION or Z_DEFLATED
   word modTime;
   word modDate;
   dword crc32;
   dword cSize;
   dword ucSize;
   word fnameLen; // Filename string follows header.
   word xtraLen;  // Extra field follows filename.
};

struct ZipFile::TZipDirHeader {
   enum {
      SIGNATURE = 0x06054b50
   };

   dword sig;
   word nDisk;
   word nStartDisk;
   word nDirEntries;
   word totalDirEntries;
   dword dirSize;
   dword dirOffset;
   word cmntLen;
};

struct ZipFile::TZipDirFileHeader {
   enum {
      SIGNATURE = 0x02014b50
   };

   dword sig;
   word verMade;
   word verNeeded;
   word flag;
   word compression; // COMP_xxxx
   word modTime;
   word modDate;
   dword crc32;
   dword cSize;   // Compressed size
   dword ucSize;  // Uncompressed size
   word fnameLen; // Filename string follows header.
   word xtraLen;  // Extra field follows filename.
   word cmntLen;  // Comment field follows extra field.
   word diskStart;
   word intAttr;
   dword extAttr;
   dword hdrOffset;

   char* GetName() const { return (char*)(this + 1); }

   char* GetExtra() const { return GetName() + fnameLen; }

   char* GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

ZipFile::ZipFile(ZipFile&& orig) noexcept
   : m_ResFileName(orig.m_ResFileName), m_InputFile(orig.m_ResFileName), m_pDirData(orig.m_pDirData), m_nEntries(orig.m_nEntries), m_papDir(orig.m_papDir)
{
   orig.m_pDirData = nullptr;
   orig.m_papDir = nullptr;
}

ZipFile& ZipFile::operator=(ZipFile&& rhs) noexcept
{
   if (this != &rhs) {
      End();
      m_ResFileName = rhs.m_ResFileName;
      m_InputFile.open(rhs.m_ResFileName);
      m_pDirData = rhs.m_pDirData;
      m_nEntries = rhs.m_nEntries;
      m_papDir = rhs.m_papDir;

      rhs.m_pDirData = nullptr;
      rhs.m_papDir = nullptr;
   }

   return *this;
}

bool ZipFile::Init(const std::string& resFileName)
{
   End();
   m_ResFileName = resFileName;

   m_InputFile.open(resFileName.c_str(), std::ifstream::binary);
   if (!m_InputFile.is_open())
      return false;

   TZipDirHeader dh{};

   m_InputFile.seekg(-(int)sizeof(dh), m_InputFile.end);
   std::streampos dhOffset = m_InputFile.tellg();
   m_InputFile.read((char*)&dh, sizeof(dh));

   if (dh.sig != TZipDirHeader::SIGNATURE)
      return false;

   m_InputFile.seekg((dword)dhOffset - dh.dirSize, std::ifstream::beg);

   // Allocate the data buffer, and read the whole thing.
   m_pDirData = new char[dh.dirSize + dh.nDirEntries * sizeof(*m_papDir)]();
   if (!m_pDirData)
      return false;

   m_InputFile.read(m_pDirData, dh.dirSize);

   // Now process each entry.
   char* pfh = m_pDirData;
   m_papDir = (const TZipDirFileHeader**)(m_pDirData + dh.dirSize);

   bool success = true;

   for (int i = 0; i < dh.nDirEntries && success; i++) {
      TZipDirFileHeader& fh = *(TZipDirFileHeader*)pfh;

      // Store the address of nth file for quicker access.
      m_papDir[i] = &fh;

      // Check the directory entry integrity.
      if (fh.sig != TZipDirFileHeader::SIGNATURE)
         success = false;
      else {
         pfh += sizeof(fh);
         pfh[fh.fnameLen] = 0;
         std::string fileName(pfh);

         // str to lower
         std::transform(fileName.begin(), fileName.end(), fileName.begin(), [](unsigned char c) { return std::tolower(c); });
         m_ZipContentsMap[fileName] = i;

         // Skip name, extra and comment fields.
         pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
      }
   }
   if (!success) {
      if (m_pDirData)
         delete[] m_pDirData;
      m_pDirData = nullptr;
   } else {
      m_nEntries = dh.nDirEntries;
   }

   return success;
}

void ZipFile::End()
{
   m_ZipContentsMap.clear();
   if (m_pDirData) {
      delete[] m_pDirData;
      m_pDirData = nullptr;
   }

   m_nEntries = 0;
   m_InputFile.close();
}

std::string ZipFile::GetFilename(int i) const
{
   std::string fileName(m_papDir[i]->GetName());
   fileName += '\0';
   return fileName;
}

int ZipFile::GetFileLen(int i) const
{
   return m_papDir[i]->ucSize;
}

// Uncompress a complete file to buffer
bool ZipFile::ReadFile(int i, void* pBuf)
{
   if (pBuf == nullptr)
      return false;

   // Quick'n dirty read, the whole file at once.
   // Ungood if the ZIP has huge files inside

   // Go to the actual file and read the local header.
   m_InputFile.seekg(m_papDir[i]->hdrOffset, std::ifstream::beg);
   TZipLocalHeader h{};

   m_InputFile.read((char*)&h, sizeof(h));
   if (h.sig != TZipLocalHeader::SIGNATURE)
      return false;

   // Skip extra fields
   m_InputFile.seekg(h.fnameLen + h.xtraLen, std::ifstream::cur);

   if (h.compression == Z_NO_COMPRESSION) {
      // Simply read in raw stored data.
      m_InputFile.read((char*)pBuf, h.cSize);
      return true;
   } else if (h.compression != Z_DEFLATED)
      return false;

   // Alloc compressed data buffer and read the whole stream
   char* pcData = new char[h.cSize]();
   if (!pcData)
      return false;

   m_InputFile.read(pcData, h.cSize);

   bool ret = true;

   // Setup the inflate stream.
   z_stream stream;
   int err;

   stream.next_in = (Bytef*)pcData;
   stream.avail_in = (uInt)h.cSize;
   stream.next_out = (Bytef*)pBuf;
   stream.avail_out = h.ucSize;
   stream.zalloc = (alloc_func)0;
   stream.zfree = (free_func)0;

   // Perform inflation. wbits < 0 indicates no zlib header inside the data.
   err = inflateInit2(&stream, -MAX_WBITS);
   if (err == Z_OK) {
      err = inflate(&stream, Z_FINISH);
      inflateEnd(&stream);
      if (err == Z_STREAM_END)
         err = Z_OK;
      inflateEnd(&stream);
   }
   if (err != Z_OK)
      ret = false;

   delete[] pcData;
   return ret;
}

int ZipFile::Find(const std::string& path) const
{
   std::string lowerCasePath = path;
   std::transform(lowerCasePath.begin(), lowerCasePath.end(), lowerCasePath.begin(), [](unsigned char c) { return std::tolower(c); });

   auto itr = m_ZipContentsMap.find(lowerCasePath);
   if (itr == m_ZipContentsMap.end())
      return -1;

   return itr->second;
}

} // namespace BIEngine
