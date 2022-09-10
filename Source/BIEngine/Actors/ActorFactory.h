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

		void AddComponentCreator(const std::string& name, ActorComponentCreator pComponentCreator) 
		{
			m_actorComponentCreators.insert({ name, pComponentCreator });
		}

		std::shared_ptr<Actor> CreateActor(tinyxml2::XMLElement* pRoot);
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