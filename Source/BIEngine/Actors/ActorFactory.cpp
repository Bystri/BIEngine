#include "ActorFactory.h"

#include <iostream>

#include <tinyxml2.h>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsTriggerComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{

	ActorFactory::ActorFactory() 
	{
		m_lastActorId = -1;

		m_actorComponentCreators[TransformComponent::g_CompId] = CreateTransformComponent;
		m_actorComponentCreators[SpriteRenderComponent::g_CompId] = CreateSpriteRenderComponent;
		m_actorComponentCreators[PhysicsComponent::g_CompId] = CreatePhysicsComponent;
		m_actorComponentCreators[PhysicsTriggerComponent::g_CompId] = CreatePhysicsTriggerComponent;
		m_actorComponentCreators[CameraComponent::g_CompId] = CreateCameraComponent;
		m_actorComponentCreators[ScriptComponent::g_CompId] = CreateScriptComponent;
	}

	std::shared_ptr<Actor> ActorFactory::CreateActor(tinyxml2::XMLElement* pRoot)
	{
		if (!pRoot) 
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Failed to create actor from given XML");
			return std::shared_ptr<Actor>();
		}

		//Создаем актера
		std::shared_ptr<Actor> pActor(new Actor(GetNextActorId()));
		if (!pActor->Init(pRoot)) {
			Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize actor from XML");
			return std::shared_ptr<Actor>();
		}

		//Загружаем компоненты
		for (tinyxml2::XMLElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
			std::shared_ptr<ActorComponent> pComponent(CreateComponent(pActor, pNode));
			if (pComponent) {
				pActor->AddComponent(pComponent);
			}
			else {
				return std::shared_ptr<Actor>();
			}
		}

		//Теперь можем инициализировать компоненты
		pActor->PostInit();
		return pActor;
	}

	std::shared_ptr<ActorComponent> ActorFactory::CreateComponent(std::shared_ptr<Actor> pActor, tinyxml2::XMLElement* pData)
	{
		std::string name(pData->Value());
		std::shared_ptr<ActorComponent> pComponent;
		auto findIt = m_actorComponentCreators.find(name);
		if (findIt != m_actorComponentCreators.end()) 
		{
			ActorComponentCreator creator = findIt->second;
			pComponent.reset(creator());
		}
		else 
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Couldn’t find ActorComponent named " + name);
			return std::shared_ptr<ActorComponent>();
		}

		pComponent->SetOwner(pActor);
		
		if (pComponent) {
			if (!pComponent->Init(pData)) {
				Logger::WriteLog(Logger::LogType::ERROR, "Component failed to initialize : " + name);
				return std::shared_ptr<ActorComponent>();
			}
		}

		return pComponent;
	}

	void ActorFactory::ModifyActor(std::shared_ptr<Actor> pActor, tinyxml2::XMLElement* overrides)
	{
		for (tinyxml2::XMLElement* pNode = overrides->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
		{
			std::string name(pNode->Value());
			auto pComponent = pActor->GetComponent<ActorComponent>(name).lock();
			if (pComponent)
				pComponent->Init(pNode);
			else
			{
				pComponent = CreateComponent(pActor, pNode);
				if (pComponent)
				{
					pActor->AddComponent(pComponent);
					pComponent->SetOwner(pActor);
					pComponent->PostInit();
				}
			}
		}
	}
}