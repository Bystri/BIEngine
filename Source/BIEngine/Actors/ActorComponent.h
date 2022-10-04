#pragma once

#include <memory>
#include <string>

#include <tinyxml2.h>

namespace BIEngine
{

	typedef std::string ComponentId;

	class Actor;

	//Основной класс для создания всех необходимых компонентов, наполняющих актеров жизнью
	class ActorComponent
	{
		friend class ActorFactory;

	public:
		ActorComponent() {}
		virtual ~ActorComponent() {}

		/*
		Компоненты могут хранить уникальные данные в виде указателей, тем более за создание полноценного актера отвечает фабрика, 
		которая контролирует этапы создания и инициализации актера и его компонентов, 
		поэтому любое копирование должно идти через ActorFactory и методы для преобразования компонентов в XML-структуру
		*/
		ActorComponent(const ActorComponent& orig) = delete;
		ActorComponent& operator=(const ActorComponent& orig) = delete;

		//В момент вызова пост-инициализации все компоненты созданы, 
		//поэтому если текущему компоненту для завершения инициализации требуются данные из другого компонента - он может смело к нему обращаться
		virtual void PostInit() { }

		//Обновление состояния компонента
		virtual void Update(float dt) {}

		//Получить XML-элемент, по которому можно воссоздать новую копию текущего компонента
		virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) = 0;

		//Стороковый идентификатор, по которому распознаются компоненты в XML-документе и редакторе
		virtual ComponentId GetComponentId() const = 0;

	private:
		void SetOwner(std::shared_ptr<Actor> pOwner) { m_pOwner = pOwner; }

	protected:
		//Так как создание/инициализацию/модификацию компонентов проводит ActorFactory, то следующие методы доступны только ему

		//Данная функция должна представлять полноценную реализацию для определения всех частей компонента и подготовки его к полноценному встраиванию в систему во время пост-инициализации
		virtual bool Init(tinyxml2::XMLElement* pData) = 0;

	protected:
		//Указатель на актера, владеюущего данным компонентом.
		//Назначается перед первичной инициализацией компонентаю
		std::shared_ptr<Actor> m_pOwner;
	};
}