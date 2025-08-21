#pragma once

#include <fstream>

#include "../StdLib/String.h"
#include "../StdLib/HashMap.h"

// Загрузчик ZIP-архивов. Оригинальный код пренадлежит Javier Arevalo.
// https://www.flipcode.com/archives/Zip_File_Loader.shtml

namespace BIEngine {

using ZipContentsMap = HashMap<String, int>;

class ZipFile {
public:
   ZipFile()
      : m_nEntries(0), m_InputFile(), m_pDirData(nullptr), m_papDir(nullptr) {}

   ~ZipFile()
   {
      End();
      m_InputFile.close();
   }

   ZipFile(ZipFile&& orig) noexcept;
   ZipFile& operator=(ZipFile&& rhs) noexcept;

   bool Init(const String& resFileName);
   void End();

   int GetNumFiles() const { return m_nEntries; }

   String GetFilename(int i) const;
   int GetFileLen(int i) const;
   bool ReadFile(int i, void* pBuf);

   int Find(const String& path) const;

   ZipContentsMap m_ZipContentsMap;

private:
   struct TZipDirHeader;
   struct TZipDirFileHeader;
   struct TZipLocalHeader;

   String m_ResFileName;
   std::ifstream m_InputFile;
   char* m_pDirData;
   int m_nEntries;

   const TZipDirFileHeader** m_papDir;
};
} // namespace BIEngine
