#pragma once

#include <map>
#include <string>

#include "../StdLib/DynamicArray.h"

namespace BIEngine {

using DipContentsMap = std::map<std::string, int>;

// Является аналогией к классу ZipFile. Нужен для того, чтобы во время инициализации проверить все файлы в папке и записать всю информацию о них в контейнер
class DirFile {
public:
   DirFile()
      : m_resFileName(), m_fileDatas(), m_dirContentsMap() {}

   ~DirFile() { End(); }

   bool Init(const std::string& resFileName);
   void End();

   int GetNumFiles() const { return m_fileDatas.Size(); }

   std::string GetFilename(int i) const;
   int GetFileLen(int i) const;
   bool ReadFile(int i, void* pBuf);

   int Find(const std::string& path) const;

private:
   struct FileData {
      std::string name;
      unsigned int size;
   };

   std::string m_resFileName;
   DynamicArray<FileData> m_fileDatas;

   DipContentsMap m_dirContentsMap;
};
} // namespace BIEngine
