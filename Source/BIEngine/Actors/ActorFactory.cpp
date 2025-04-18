﻿#include "ActorFactory.h"

#include <iostream>

#include <tinyxml2.h>

#include "TransformComponent.h"
#include "LightComponent.h"
#include "RenderComponent.h"
#include "AnimationComponent.h"
#include "Physics2DComponent.h"
#include "Physics2DTriggerComponent.h"
#include "Physics3DComponent.h"
#include "NavAgentComponent.h"
#include "ScriptComponent.h"
#include "MeshComponent.h"
#include "SkinnedMeshComponent.h"
#include "SkeletonComponent.h"
#include "BoneComponent.h"
#include "PlayerComponent.h"
#include "../Utilities/Logger.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

ActorFactory::ActorFactory()
{
   m_lastActorId = -1;

   // Сюда должны добавлять все функции-конструкторы компонентов актера
   m_actorComponentCreators[TransformComponent::g_CompId] = CreateTransformComponent;
   m_actorComponentCreators[DirectionalLightComponent::g_CompId] = CreateDirectionalLightComponent;
   m_actorComponentCreators[PointLightComponent::g_CompId] = CreatePointLightComponent;
   m_actorComponentCreators[SpotLightComponent::g_CompId] = CreateSpotLightComponent;
   m_actorComponentCreators[SpriteRenderComponent::g_CompId] = CreateSpriteRenderComponent;
   m_actorComponentCreators[BoxRenderComponent::g_CompId] = CreateBoxRenderComponent;
   m_actorComponentCreators[SphereRenderComponent::g_CompId] = CreateSphereRenderComponent;
   m_actorComponentCreators[ModelRenderComponent::g_CompId] = CreateModelRenderComponent;
   m_actorComponentCreators[Physics2DComponent::g_CompId] = CreatePhysics2DComponent;
   m_actorComponentCreators[Physics2DTriggerComponent::g_CompId] = CreatePhysics2DTriggerComponent;
   m_actorComponentCreators[Physics3DComponent::g_CompId] = CreatePhysics3DComponent;
   m_actorComponentCreators[NavAgentComponent::g_CompId] = CreateNavAgentComponent;
   m_actorComponentCreators[ScriptComponent::g_CompId] = CreateScriptComponent;
   m_actorComponentCreators[AnimationComponent::g_CompId] = CreateAnimatorComponent;
   m_actorComponentCreators[MeshComponent::g_CompId] = CreateMeshComponent;
   m_actorComponentCreators[SkinnedMeshComponent::g_CompId] = CreateSkinnedMeshComponent;
   m_actorComponentCreators[SkeletonComponent::g_CompId] = CreateSkeletonComponent;
   m_actorComponentCreators[BoneComponent::g_CompId] = CreateBoneComponent;
   m_actorComponentCreators[PlayerComponent::g_CompId] = CreateBIPlayerComponent;
}

std::shared_ptr<Actor> ActorFactory::CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition, const glm::vec3* const pRotation, Actor* const pParent)
{
   // Create actor
   std::shared_ptr<Actor> pActor = CreateActorFromRootElement(pRoot, pParent);
   if (pActor == nullptr) {
      return pActor;
   }

   // Hack for change position before another components can read it
   auto pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

   if (pPosition) {
      pTransformComponent->SetLocalPosition(*pPosition);
   }

   if (pRotation) {
      pTransformComponent->SetLocalRotation(*pRotation);
   }

   // Init components
   if (g_pApp->m_pGameLogic->IsLevelLoaded()) {
      pActor->OnLevelLoaded();
      pActor->Activate();
   }

   return pActor;
}

std::shared_ptr<Actor> ActorFactory::CreateActorFromRootElement(tinyxml2::XMLElement* pRoot, Actor* const pParent)
{
   if (!pRoot) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to create actor from null XML-element");
      return nullptr;
   }

   // Create actor
   std::shared_ptr<Actor> pActor = std::shared_ptr<Actor>(new Actor(GetNextActorId()));

   pActor->m_pParent = pParent;

   if (!pActor->Init(pRoot)) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize actor from XML");
      return nullptr;
   }

   // Load components
   tinyxml2::XMLElement* const pComponentsElement = pRoot->FirstChildElement("Components");
   if (pComponentsElement) {
      for (tinyxml2::XMLElement* pNode = pComponentsElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
         std::shared_ptr<ActorComponent> pComponent(CreateComponent(pActor, pNode));
         if (pComponent) {
            pActor->AddComponent(pComponent);
         } else {
            return nullptr;
         }
      }
   }

   tinyxml2::XMLElement* const pChildrenElement = pRoot->FirstChildElement("Children");
   if (pChildrenElement) {
      for (tinyxml2::XMLElement* pNode = pChildrenElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
         pActor->AddChild(CreateActorFromRootElement(pNode, pActor.get()));
      }
   }

   return pActor;
}

std::shared_ptr<ActorComponent> ActorFactory::CreateComponent(std::shared_ptr<Actor> pActor, tinyxml2::XMLElement* pData)
{
   // Создаем нового актера
   std::string name(pData->Value());
   std::shared_ptr<ActorComponent> pComponent;
   auto findIt = m_actorComponentCreators.find(name);
   if (findIt != m_actorComponentCreators.end()) {
      ActorComponentCreator creator = findIt->second;
      pComponent = creator();
   } else {
      Logger::WriteLog(Logger::LogType::ERROR, "Couldn’t find ActorComponent named " + name);
      return std::shared_ptr<ActorComponent>();
   }

   // Назначаем владельца компонента
   pComponent->SetOwner(pActor.get());

   // Инициализируем компонент
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
   tinyxml2::XMLElement* const pComponents = overrides->FirstChildElement("Components");
   for (tinyxml2::XMLElement* pNode = pComponents->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      std::string name(pNode->Value());
      auto pComponent = pActor->GetComponent<ActorComponent>(name).lock();
      if (pComponent)
         pComponent->Init(pNode);
      else {
         pComponent = CreateComponent(pActor, pNode);
         if (pComponent) {
            pActor->AddComponent(pComponent);
            pComponent->Activate();
         }
      }
   }
}
} // namespace BIEngine
