#pragma once

#include <map>
#include <string>

#include <glm/glm.hpp>

#include "Actor.h"

namespace BIEngine
{

	typedef ActorComponent* (*ActorComponentCreator)();
	typedef std::map<std::string, ActorComponentCreator> ActorComponentCreatorMap;

	class ActorFactory
	{
	public:
		ActorFactory();

		//Так как фабрика отвечает за создание и назначение актерам уникального идентификатора, по которому все системы будут ссылаться на данного актера, 
		//копировать фабрики запрещено, по причине того, что тогда две фабрики будут выдавать актерам одинаковые ID.
		ActorFactory(const ActorFactory& orig) = delete;
		//Так как фабрика отвечает за создание и назначение актерам уникального идентификатора, по которому все системы будут ссылаться на данного актера, 
		//копировать фабрики запрещено, по причине того, что тогда две фабрики будут выдавать актерам одинаковые ID.
		ActorFactory& operator=(const ActorFactory& orig) = delete;

		void AddComponentCreator(const std::string& name, ActorComponentCreator pComponentCreator) 
		{
			m_actorComponentCreators.insert({ name, pComponentCreator });
		}

		//Создает актера со всеми требуемыми компонентами на основе XML-элемента.
		//Возвращает нулевой указатель, если требуется.
		std::shared_ptr<Actor> CreateActor(tinyxml2::XMLElement* pRoot);

		//Модифицирует актера на основе предоставленного XML-элемента
		//В предоставленном элементе может быть полное или частичное описание уже имеющегося компонента актера - тогда он просто будет обновлен
		//Также можно передовать полноценное описание нового компонента, такой компонент будет корректно создан, проинициализирован и добавлен актеру
		void ModifyActor(std::shared_ptr<Actor> pActor, tinyxml2::XMLElement* pOverrides);

	protected:
		//Создает компонент на основе информации из XML-структуры. Вернет нулевой указатель, если данный компонент не может быть создан
		virtual std::shared_ptr<ActorComponent> CreateComponent(std::shared_ptr<Actor> pActor, tinyxml2::XMLElement* pData);

	private:
		ActorId GetNextActorId() { ++m_lastActorId; return m_lastActorId; }

	protected:
		ActorComponentCreatorMap m_actorComponentCreators;

	private:
		ActorId m_lastActorId;
	};
}