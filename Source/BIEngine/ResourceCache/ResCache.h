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

	//Базовый класс для загрузкика ресурса. Он хранит всю информацию по работе с памятью файла, связанного с ресурсом, и его инициализации
	class IResourceLoader
	{
	public:
		virtual ~IResourceLoader() {}

		//Wildcard паттерн, по которому будут искаться ресурсы в архиве
		virtual std::string GetPattern() = 0;
		//Данный флаг сигнализирует о том, что ресурсу не нужна инициализация и он может использоваться в "сыром" виде. Если данный флаг имеет значение "true", то значение DiscardRawBufferAfterLoad() не учитвается 
		virtual bool UseRawFile() = 0;
		//Удалить буфер после загрузки ресурса. Требуется делать в том случае, если на основе ресурса создается класс или куда-то копируется после инициализации, например в память видеокарты
		virtual bool DiscardRawBufferAfterLoad() = 0;
		//Требуется ли поместить дополнитульный нуль в дополнительный конечный байт памяти файла после загрузки
		virtual bool AddNullZero() { return false; }
		//Размер загруженного ресурса в байтах
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) = 0;
		//Процедура загрузки ресурса
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) = 0;
	};


	//Описывает файл, из которого будут загружаться ресурсы. Ими могут быть, например, обычные папки или сжатые архивы
	class IResourceFile
	{
	public:
		virtual ~IResourceFile() { }

		virtual bool Open() = 0;
		//Получить сырой размер файла, находящегося в хранилище
		virtual int GetRawResourceSize(const const std::string& resName) = 0;
		//Заполняет буфер неинициализированными данными ресурса
		virtual int GetRawResource(const const std::string& resName, char* pBuffer) = 0;
		//Общее количество ресурсов в хранилище
		virtual int GetNumResources() const = 0;
		//Получить имя ресурса, который имеет порядковый номер num, который начинается с 0
		virtual std::string GetResourceName(int num) const = 0;
	};


	class IResourceExtraData
	{
	public:
		virtual ~IResourceExtraData() { }

		virtual std::string ToString() = 0;
	};

	//Zip-архив, хранящий все необходимые ресурсы для работы
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

	//Обычная папка, хранящая все неоходимые ресурсы для работы
	//В частности нужна во время разработки игры или для редактора
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


	//Содержит полные данные загруженного ресурса: размер, ссылку на буфер с данными ресурса и т.п.
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
		std::shared_ptr<IResourceExtraData> m_pExtra; //Дополнительная информация, которая может быть уникальна для каждого типа ресурса.
		ResCache* m_pResCache;
	};

	//Стандартный загрузчик
	class DefaultResourceLoader : public IResourceLoader
	{
	public:
		virtual bool UseRawFile() { return true; }
		//Не учитвается, так как UseRawFile возвращает значение "true"
		virtual bool DiscardRawBufferAfterLoad() { return true; }
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) { return rawSize; }
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) { return true; }
		//Подходит для всех файлов
		virtual std::string GetPattern() { return "*"; }

	};

	typedef std::list<std::shared_ptr<ResHandle>> ResHandleList;
	typedef std::map<std::string, std::shared_ptr<ResHandle>> ResHandleMap;
	typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaders;

	//ResCache основан на паттерне Singleton (да простят меня Боги)
	//Перед обращением к кэшу ресурсов его необходимо явно создать вызовом функции Create, чтобы кэш имел путь к хранилищу ресурсов и знал максимальный размер доступной для загруженных ресурсов памяти
	class ResCache
	{
		friend class ResHandle;

		static ResCache* s_pSingleton;

	public:
		static bool Create(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> pFile);
		static void Destroy();
		//Возвращает указетель на кэш ресурсов. Перед использованием необходимо выполнить инициализацию с помощью фукнции ResCache::Create
		static ResCache* Get() { assert(s_pSingleton); return s_pSingleton; }

		bool Init();
		void RegisterLoader(std::shared_ptr<IResourceLoader> pLoader);

		//Возвращает держатель ресурса со всей необходимой информацией для дальнейшей работой с данным ресурсом. Если файл не был загружен, внутри происходит неявная загрузка и, если требуется, свобождения памяти
		std::shared_ptr<ResHandle> GetHandle(const std::string& resName);
		//Возвращает список файлов, чье название внутри хранилища ресурсов подходит под заданный паттерн 
		std::vector<std::string> Match(const std::string& pattern);

	protected:
		//Освобожает место посредством освобождения из памяти самых редкоиспользуемых ресурсов
		//Возвращает false, если нет никакой возможности освободить достаточно места
		bool MakeRoom(unsigned int size);
		char* Allocate(unsigned int size);
		//Освобождает ресурс из памяти
		void Free(std::shared_ptr<ResHandle> pGonner);

		//Загрузка ресурса из хранилища
		std::shared_ptr<ResHandle> Load(const std::string& resName);
		//Поиск уже загруженного ресурса, находящегося в памяти
		std::shared_ptr<ResHandle> Find(const std::string& resName);
		//Выносит ресурс, к которому обратились через метод GetHandle, на первую строчку LRU кэша
		void Update(std::shared_ptr<ResHandle> pHandle);

		//Освобождает самый редкоиспользуемый ресурс
		void FreeOneResource();
		//Вызывается из деструктура ResHandle, что сигнализирует о том, что ресурс явно уничтожен
		void MemoryHasBeenFreed(unsigned int size);

	private:
		ResCache(const unsigned int sizeInMb, std::shared_ptr<IResourceFile> pFile);
		virtual ~ResCache();

	private:
		ResHandleList m_lru;
		ResHandleMap m_resources;
		ResourceLoaders m_resourceLoaders;

		std::shared_ptr<IResourceFile> m_pFile; //Указатель на файл, который является хранилищем ресурсов

		unsigned int			m_cacheSize;			//Максимальный доступный размер кэша 
		unsigned int			m_allocated;			//Текущий размер кэша
	};

}