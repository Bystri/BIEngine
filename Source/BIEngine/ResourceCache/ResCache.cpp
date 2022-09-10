#include "ResCache.h"

#include <algorithm>
#include <iostream>
#include <cctype>

#include "../Utilities/String.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{

	ResCache* ResCache::s_pSingleton = nullptr;

	bool ResCache::Create(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> pFile)
	{
		if (s_pSingleton)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Overwriting ResCache singleton");
			delete s_pSingleton;
			s_pSingleton = nullptr;
		}

		s_pSingleton = new ResCache(sizeInMb, pFile);
		if (s_pSingleton)
			return s_pSingleton->Init();

		return false;
	}

	void ResCache::Destroy()
	{
		assert(s_pSingleton);
		if (s_pSingleton)
		{
			delete s_pSingleton;
			s_pSingleton = nullptr;
		}
	}


	/***ResourceZipFile***/
	ResourceZipFile::~ResourceZipFile()
	{
		if (m_pZipFile) {
			delete m_pZipFile;
			m_pZipFile = nullptr;
		}
	}

	bool ResourceZipFile::Open()
	{
		m_pZipFile = new ZipFile;
		if (m_pZipFile)
			return m_pZipFile->Init(m_resFileName.c_str());

		return false;
	}

	int ResourceZipFile::GetRawResourceSize(const std::string& resName)
	{
		int resourceNum = m_pZipFile->Find(resName.c_str());
		if (resourceNum == -1)
			return -1;

		return m_pZipFile->GetFileLen(resourceNum);
	}

	int ResourceZipFile::GetRawResource(const std::string& resName, char* pBuffer)
	{
		int size = 0;
		int resourceNum = m_pZipFile->Find(resName.c_str());
		if (resourceNum != -1)
		{
			size = m_pZipFile->GetFileLen(resourceNum);
			m_pZipFile->ReadFile(resourceNum, pBuffer);
		}
		return size;
	}

	int ResourceZipFile::GetNumResources() const
	{
		return (m_pZipFile == nullptr) ? 0 : m_pZipFile->GetNumFiles();
	}

	std::string ResourceZipFile::GetResourceName(int num) const
	{
		std::string resName = "";
		if (m_pZipFile != nullptr && num >= 0 && num < m_pZipFile->GetNumFiles())
		{
			resName = m_pZipFile->GetFilename(num);
		}
		return resName;
	}


	/***ResourceDirFile***/
	ResourceDirFile::~ResourceDirFile()
	{
	}

	bool ResourceDirFile::Open()
	{
		m_pDirFile = new DirFile;
		if (m_pDirFile)
			return m_pDirFile->Init(m_resFileName.c_str());

		return false;
	}

	int ResourceDirFile::GetRawResourceSize(const std::string& resName)
	{
		int resourceNum = m_pDirFile->Find(resName.c_str());
		if (resourceNum == -1)
			return -1;

		return m_pDirFile->GetFileLen(resourceNum);
	}

	int ResourceDirFile::GetRawResource(const std::string& resName, char* pBuffer)
	{
		int size = 0;
		int resourceNum = m_pDirFile->Find(resName.c_str());
		if (resourceNum != -1)
		{
			size = m_pDirFile->GetFileLen(resourceNum);
			m_pDirFile->ReadFile(resourceNum, pBuffer);
		}
		return size;
	}

	int ResourceDirFile::GetNumResources() const
	{
		return (m_pDirFile == nullptr) ? 0 : m_pDirFile->GetNumFiles();
	}

	std::string ResourceDirFile::GetResourceName(int num) const
	{
		std::string resName = "";
		if (m_pDirFile != nullptr && num >= 0 && num < m_pDirFile->GetNumFiles())
		{
			resName = m_pDirFile->GetFilename(num);
		}
		return resName;
	}


	/***ResHandle***/
	ResHandle::ResHandle(const std::string& resName, char* pBuffer, unsigned int size, ResCache* pResCache)
		: m_resName(resName)
		, m_pBuffer(pBuffer)
		, m_size(size)
		, m_pExtra(nullptr)
		, m_pResCache(pResCache)
	{
	}

	ResHandle::~ResHandle()
	{
		if (m_pBuffer) 
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
		}

		//Явно уменьшает значение использованной памяти в кэше
		m_pResCache->MemoryHasBeenFreed(m_size);
	}

	ResCache::ResCache(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> resFile)
		: m_cacheSize(sizeInMb * 1024 * 1024)
		, m_allocated(0)
		, m_pFile(resFile)
	{
	}

	ResCache::~ResCache()
	{
		while (!m_lru.empty())
		{
			FreeOneResource();
		}
	}

	bool ResCache::Init()
	{
		bool retValue = false;
		if (m_pFile->Open())
		{
			RegisterLoader(std::make_shared<DefaultResourceLoader>());
			retValue = true;
		}
		return retValue;
	}

	void ResCache::RegisterLoader(std::shared_ptr<IResourceLoader> pLoader)
	{
		m_resourceLoaders.push_front(pLoader);
	}

	std::shared_ptr<ResHandle> ResCache::GetHandle(const std::string& resName)
	{
		std::shared_ptr<ResHandle> pHandle(Find(resName));
		if (pHandle == nullptr)
		{
			pHandle = Load(resName);
			assert(pHandle);
		}
		else
		{
			Update(pHandle);
		}
		return pHandle;
	}

	std::vector<std::string> ResCache::Match(const std::string& pattern)
	{
		std::vector<std::string> matchingNames;
		if (m_pFile == nullptr)
			return matchingNames;

		int numFiles = m_pFile->GetNumResources();
		for (int i = 0; i < numFiles; ++i)
		{
			std::string name = m_pFile->GetResourceName(i);
			std::transform(name.begin(), name.end(), name.begin(), (int(*)(int)) std::tolower);
			if (WildcardMatch(pattern.c_str(), name.c_str()))
			{
				matchingNames.push_back(name);
			}
		}
		return matchingNames;
	}

	std::shared_ptr<ResHandle> ResCache::Load(const std::string& resName)
	{
		std::shared_ptr<IResourceLoader> pLoader;
		std::shared_ptr<ResHandle> pHandle;

		for (ResourceLoaders::iterator it = m_resourceLoaders.begin(); it != m_resourceLoaders.end(); ++it)
		{
			std::shared_ptr<IResourceLoader> pTestLoader = *it;

			if (WildcardMatch(pTestLoader->GetPattern().c_str(), resName.c_str()))
			{
				pLoader = pTestLoader;
				break;
			}
		}

		if (!pLoader)
		{
			assert(pLoader && "Default resource loader not found!");
			return pHandle;
		}

		int rawSize = m_pFile->GetRawResourceSize(resName);
		if (rawSize < 0)
		{
			assert(rawSize > 0 && "Resource size returned -1 - Resource not found");
			return std::shared_ptr<ResHandle>();
		}

		int allocSize = rawSize + ((pLoader->AddNullZero()) ? (1) : (0));

		//Если мы используем сырой файл, то он сразу занимает место в кэше. Если нет, то мы сначала выделяем под файл временную память, чтобы использовать ее для инициализации ресурса
		char* pRawBuffer = pLoader->UseRawFile() ? Allocate(allocSize) : new char[allocSize]();

		if (pRawBuffer == nullptr || m_pFile->GetRawResource(resName, pRawBuffer) == 0)
		{
			//Место под ресурс не может быть выделено!
			return std::shared_ptr<ResHandle>();
		}

		char* pBuffer = nullptr;
		unsigned int size = 0;

		if (pLoader->UseRawFile())
		{
			//Если мы можем использовать сырой файл как ресурс, то мы попросту создает на его основе держатель ресурса и возвращаем его
			pBuffer = pRawBuffer;
			pHandle = std::shared_ptr<ResHandle>(new ResHandle(resName, pBuffer, rawSize, this));
		}
		else
		{
			size = pLoader->GetLoadedResourceSize(pRawBuffer, rawSize);
			pBuffer = Allocate(size);
			if (pRawBuffer == nullptr || pBuffer == nullptr)
			{
				//Место под ресурс не может быть выделено!
				return std::shared_ptr<ResHandle>();
			}
			pHandle = std::shared_ptr<ResHandle>(new ResHandle(resName, pBuffer, size, this));

			//Используем ранее загруженный сырой файл, чтобы на его основе инициализировать ресурс и поместить указатель на инициализированную память в buffer
			bool success = pLoader->LoadResource(pRawBuffer, rawSize, pHandle);

			if (pLoader->DiscardRawBufferAfterLoad())
			{
				if (pRawBuffer) {
					delete[] pRawBuffer;
					pRawBuffer = nullptr;
				}
			}

			if (!success)
			{
				//Место под ресурс не может быть выделено!
				return std::shared_ptr<ResHandle>();
			}
		}

		//Обновляем LRU кэш
		if (pHandle)
		{
			m_lru.push_front(pHandle);
			m_resources[resName] = pHandle;
		}

		return pHandle;
	}

	std::shared_ptr<ResHandle> ResCache::Find(const std::string& resName)
	{
		auto itr = m_resources.find(resName);
		if (itr == m_resources.end())
			return std::shared_ptr<ResHandle>();

		return itr->second;
	}

	void ResCache::Update(std::shared_ptr<ResHandle> pHandle)
	{
		m_lru.remove(pHandle);
		m_lru.push_front(pHandle);
	}

	char* ResCache::Allocate(unsigned int size)
	{
		if (!MakeRoom(size))
			return nullptr;

		char* pMem = new char[size];
		if (pMem)
			m_allocated += size;

		return pMem;
	}

	void ResCache::FreeOneResource()
	{
		m_resources.erase(m_lru.back()->GetName());
		m_lru.pop_back();
		//Мы не уменьшает значение использованной памяти, так как кто-то еще может использовать ресурс.
		//Память освободится тогда, когда умный указатель на держатель ресурса вызовет деструктор
	}

	bool ResCache::MakeRoom(unsigned int size)
	{
		if (size > m_cacheSize)
			return false;

		while (size > (m_cacheSize - m_allocated))
		{
			//Может быть такое, что список задейтсовванных ресурсов пуст, а память не освободилась.
			//Это происходит из-за того, что ресурс еще кем-то используется и память не может быть освобождена
			if (m_lru.empty())
				return false;

			FreeOneResource();
		}

		return true;
	}

	void ResCache::Free(std::shared_ptr<ResHandle> pGonner)
	{
		m_lru.remove(pGonner);
		m_resources.erase(pGonner->GetName());
		//Мы не уменьшает значение использованной памяти, так как кто-то еще может использовать ресурс.
		//Память освободится тогда, когда умный указатель на держатель ресурса вызовет деструктор
	}

	void ResCache::MemoryHasBeenFreed(unsigned int size)
	{
		m_allocated -= size;
	}

}