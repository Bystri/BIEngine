#pragma once

#include <map>
#include <string>
#include <vector>

namespace BIEngine
{

	typedef std::map<std::string, int> DipContentsMap;

	//явл€етс€ аналогией к классу ZipFile. Ќужен дл€ того, чтобы во врем€ инициализации проверить все файлы в папке и записать всю информацию о них в контейнер
	class DirFile {
	public:
		DirFile() : m_resFileName(), m_fileDatas(), m_dirContentsMap() {}
		~DirFile() { End();}

		bool Init(const std::string& resFileName);
		void End();

		int GetNumFiles() const { return m_fileDatas.size(); }
		std::string GetFilename(int i) const;
		int GetFileLen(int i) const;
		bool ReadFile(int i, void* pBuf);

		int Find(const std::string& path) const;

	private:
		struct FileData
		{
			std::string name;
			unsigned int size;
		};

		std::string m_resFileName;
		std::vector<FileData> m_fileDatas;

		DipContentsMap m_dirContentsMap;
	};
}