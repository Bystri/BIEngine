#pragma once

#include "../StdLib/String.h"
#include "../StdLib/HashMap.h"
#include "../StdLib/DynamicArray.h"

namespace BIEngine {

using DipContentsMap = HashMap<String, int>;

// Является аналогией к классу ZipFile. Нужен для того, чтобы во время инициализации проверить все файлы в папке и записать всю информацию о них в контейнер
class DirFile {
public:
   DirFile()
      : m_resFileName(), m_fileDatas(), m_dirContentsMap() {}

   ~DirFile() { End(); }

   bool Init(const String& resFileName);
   void End();

   int GetNumFiles() const { return m_fileDatas.Size(); }

   String GetFilename(int i) const;
   int GetFileLen(int i) const;
   bool ReadFile(int i, void* pBuf);

   int Find(const String& path) const;

private:
   struct FileData {
      String name;
      unsigned int size;
   };

   String m_resFileName;
   DynamicArray<FileData> m_fileDatas;

   DipContentsMap m_dirContentsMap;
};
} // namespace BIEngine
