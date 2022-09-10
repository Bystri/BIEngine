#pragma once

#include <memory>
#include <list>
#include <vector>
#include <cassert>

#include "ZipFile.h"
#include "DirFile.h"

namespace BIEngine
{

	class ResHandle;
	class ResCache;

	//������� ����� ��� ���������� �������. �� ������ ��� ���������� �� ������ � ������� �����, ���������� � ��������, � ��� �������������
	class IResourceLoader
	{
	public:
		virtual ~IResourceLoader() {}

		//Wildcard �������, �� �������� ����� �������� ������� � ������
		virtual std::string GetPattern() = 0;
		//������ ���� ������������� � ���, ��� ������� �� ����� ������������� � �� ����� �������������� � "�����" ����. ���� ������ ���� ����� �������� "true", �� �������� DiscardRawBufferAfterLoad() �� ���������� 
		virtual bool UseRawFile() = 0;
		//������� ����� ����� �������� �������. ��������� ������ � ��� ������, ���� �� ������ ������� ��������� ����� ��� ����-�� ���������� ����� �������������, �������� � ������ ����������
		virtual bool DiscardRawBufferAfterLoad() = 0;
		//��������� �� ��������� �������������� ���� � �������������� �������� ���� ������ ����� ����� ��������
		virtual bool AddNullZero() { return false; }
		//������ ������������ ������� � ������
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) = 0;
		//��������� �������� �������
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) = 0;
	};


	//��������� ����, �� �������� ����� ����������� �������. ��� ����� ����, ��������, ������� ����� ��� ������ ������
	class IResourceFile
	{
	public:
		virtual ~IResourceFile() { }

		virtual bool Open() = 0;
		//�������� ����� ������ �����, ������������ � ���������
		virtual int GetRawResourceSize(const const std::string& resName) = 0;
		//��������� ����� ��������������������� ������� �������
		virtual int GetRawResource(const const std::string& resName, char* pBuffer) = 0;
		//����� ���������� �������� � ���������
		virtual int GetNumResources() const = 0;
		//�������� ��� �������, ������� ����� ���������� ����� num, ������� ���������� � 0
		virtual std::string GetResourceName(int num) const = 0;
	};


	class IResourceExtraData
	{
	public:
		virtual ~IResourceExtraData() { }

		virtual std::string ToString() = 0;
	};

	//Zip-�����, �������� ��� ����������� ������� ��� ������
	class ResourceZipFile : public IResourceFile
	{
	public:
		ResourceZipFile(const std::string resFileName) :m_pZipFile(nullptr), m_resFileName(resFileName) { }
		virtual ~ResourceZipFile();

		virtual bool Open();
		virtual int GetRawResourceSize(const std::string& resName);
		virtual int GetRawResource(const const std::string& resName, char* pBuffer);
		virtual int GetNumResources() const;
		virtual std::string GetResourceName(int num) const;

	private:
		ZipFile* m_pZipFile;
		std::string m_resFileName;
	};

	//������� �����, �������� ��� ���������� ������� ��� ������
	//� ��������� ����� �� ����� ���������� ���� ��� ��� ���������
	class ResourceDirFile : public IResourceFile
	{
	public:
		ResourceDirFile(const std::string resFileName) :m_pDirFile(nullptr), m_resFileName(resFileName) { }
		virtual ~ResourceDirFile();

		virtual bool Open();
		virtual int GetRawResourceSize(const std::string& resName);
		virtual int GetRawResource(const const std::string& resName, char* pBuffer);
		virtual int GetNumResources() const;
		virtual std::string GetResourceName(int num) const;

	private:
		DirFile* m_pDirFile;
		std::string m_resFileName;
	};


	//�������� ������ ������ ������������ �������: ������, ������ �� ����� � ������� ������� � �.�.
	class ResHandle
	{
		friend class ResCache;

	public:
		ResHandle(const std::string& resName, char* pBuffer, unsigned int size, ResCache* pResCache);
		virtual ~ResHandle();

		const std::string GetName() { return m_resName; }
		unsigned int Size() const { return m_size; }
		char* Buffer() const { return m_pBuffer; }
		char* WritableBuffer() { return m_pBuffer; }

		std::shared_ptr<IResourceExtraData> GetExtra() { return m_pExtra; }
		void SetExtra(std::shared_ptr<IResourceExtraData> pExtra) { m_pExtra = pExtra; }

	protected:
		const std::string m_resName;

		char* m_pBuffer;
		unsigned int m_size;
		std::shared_ptr<IResourceExtraData> m_pExtra; //�������������� ����������, ������� ����� ���� ��������� ��� ������� ���� �������.
		ResCache* m_pResCache;
	};

	//����������� ���������
	class DefaultResourceLoader : public IResourceLoader
	{
	public:
		virtual bool UseRawFile() { return true; }
		//�� ����������, ��� ��� UseRawFile ���������� �������� "true"
		virtual bool DiscardRawBufferAfterLoad() { return true; }
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) { return rawSize; }
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) { return true; }
		//�������� ��� ���� ������
		virtual std::string GetPattern() { return "*"; }

	};

	typedef std::list<std::shared_ptr<ResHandle>> ResHandleList;
	typedef std::map<std::string, std::shared_ptr<ResHandle>> ResHandleMap;
	typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaders;

	//ResCache ������� �� �������� Singleton (�� ������� ���� ����)
	//����� ���������� � ���� �������� ��� ���������� ���� ������� ������� ������� Create, ����� ��� ���� ���� � ��������� �������� � ���� ������������ ������ ��������� ��� ����������� �������� ������
	class ResCache
	{
		friend class ResHandle;

		static ResCache* s_pSingleton;

	public:
		static bool Create(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> pFile);
		static void Destroy();
		//���������� ��������� �� ��� ��������. ����� �������������� ���������� ��������� ������������� � ������� ������� ResCache::Create
		static ResCache* Get() { assert(s_pSingleton); return s_pSingleton; }

		bool Init();
		void RegisterLoader(std::shared_ptr<IResourceLoader> pLoader);

		//���������� ��������� ������� �� ���� ����������� ����������� ��� ���������� ������� � ������ ��������. ���� ���� �� ��� ��������, ������ ���������� ������� �������� �, ���� ���������, ����������� ������
		std::shared_ptr<ResHandle> GetHandle(const std::string& resName);
		//���������� ������ ������, ��� �������� ������ ��������� �������� �������� ��� �������� ������� 
		std::vector<std::string> Match(const std::string& pattern);

	protected:
		//���������� ����� ����������� ������������ �� ������ ����� ����������������� ��������
		//���������� false, ���� ��� ������� ����������� ���������� ���������� �����
		bool MakeRoom(unsigned int size);
		char* Allocate(unsigned int size);
		//����������� ������ �� ������
		void Free(std::shared_ptr<ResHandle> pGonner);

		//�������� ������� �� ���������
		std::shared_ptr<ResHandle> Load(const std::string& resName);
		//����� ��� ������������ �������, ������������ � ������
		std::shared_ptr<ResHandle> Find(const std::string& resName);
		//������� ������, � �������� ���������� ����� ����� GetHandle, �� ������ ������� LRU ����
		void Update(std::shared_ptr<ResHandle> pHandle);

		//����������� ����� ����������������� ������
		void FreeOneResource();
		//���������� �� ����������� ResHandle, ��� ������������� � ���, ��� ������ ���� ���������
		void MemoryHasBeenFreed(unsigned int size);

	private:
		ResCache(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> pFile);
		virtual ~ResCache();

	private:
		ResHandleList m_lru;
		ResHandleMap m_resources;
		ResourceLoaders m_resourceLoaders;

		std::shared_ptr<IResourceFile> m_pFile; //��������� �� ����, ������� �������� ���������� ��������

		unsigned int			m_cacheSize;			//������������ ��������� ������ ���� 
		unsigned int			m_allocated;			//������� ������ ����
	};

}