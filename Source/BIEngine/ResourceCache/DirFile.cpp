#include "DirFile.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>

namespace BIEngine
{
	bool DirFile::Init(const std::string& resFileName)
	{
		End();
		m_resFileName = resFileName;

		const std::filesystem::path resPath{ resFileName };

		if (resPath.empty())
			return false;
		
		int i = 0;

		for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(resPath))
		{
			std::string path = std::filesystem::relative(dir_entry.path(), resPath).string();

			//Меняем разделите директорий Windows на разделители в стиле POSIX
			//Топорный метод, из-за того, что в POSIX системах символ '\' - является корретным символо в имени файла и мы, тем самым, можем его из-за этого переименовать
			//TODO: если этот проект доберется до сборки под Linux, надо ограничеть этот кусок ifdefine или чем-то похожим или молится, что в CPP2X дадут возможность выбирать стиль разделителей
			std::replace(path.begin(), path.end(), '\\', '/');

			std::transform(path.begin(), path.end(), path.begin(), [](unsigned char c) {return std::tolower(c); });

			m_fileDatas.push_back({ path, static_cast<unsigned int>(dir_entry.file_size()) });
			m_dirContentsMap[path] = i++;
		}

		return true;
	}

	void DirFile::End()
	{
		m_dirContentsMap.clear();
	}

	std::string DirFile::GetFilename(int i) const
	{
		assert(i >= 0 && i < m_fileDatas.size());
		return m_fileDatas[i].name;
	}

	int DirFile::GetFileLen(int i) const
	{
		assert(i >= 0 && i < m_fileDatas.size());
		return m_fileDatas[i].size;
	}

	bool DirFile::ReadFile(int i, void* pBuf)
	{
		assert(i >= 0 && i < m_fileDatas.size());

		if (pBuf == nullptr)
			return false;

		//Просто читаем весь файл в буфер
		std::ifstream inputFile(m_resFileName + "/" + m_fileDatas[i].name, std::ifstream::binary);
		if (inputFile.read((char*)pBuf, m_fileDatas[i].size))
			return true;
		
		return false;
	}

	int DirFile::Find(const std::string& path) const
	{
		std::string lowerCasePath = path;
		std::transform(lowerCasePath.begin(), lowerCasePath.end(), lowerCasePath.begin(), [](unsigned char c) {return std::tolower(c); });

		auto itr = m_dirContentsMap.find(lowerCasePath);
		if (itr == m_dirContentsMap.end())
			return -1;

		return itr->second;
	}

}