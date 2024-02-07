#include "ActorFactory.h"

#include <iostream>

#include <tinyxml2.h>

#include "TransformComponent.h"
#include "LightComponent.h"
#include "RenderComponent.h"
#include "Physics2DComponent.h"
#include "Physics2DTriggerComponent.h"
#include "Physics3DComponent.h"
#include "Physics3DTriggerComponent.h"
#include "NavAgentComponent.h"
#include "ScriptComponent.h"
#include "../Utilities/Logger.h"

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
   m_actorComponentCreators[ModelRenderComponent::g_CompId] = CreateModelRenderComponent;
   m_actorComponentCreators[Physics2DComponent::g_CompId] = CreatePhysics2DComponent;
   m_actorComponentCreators[Physics2DTriggerComponent::g_CompId] = CreatePhysics2DTriggerComponent;
   m_actorComponentCreators[Physics3DComponent::g_CompId] = CreatePhysics3DComponent;
   m_actorComponentCreators[Physics3DTriggerComponent::g_CompId] = CreatePhysics3DTriggerComponent;
   m_actorComponentCreators[NavAgentComponent::g_CompId] = CreateNavAgentComponent;
   m_actorComponentCreators[ScriptComponent::g_CompId] = CreateScriptComponent;
}

std::shared_ptr<Actor> ActorFactory::CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition, const glm::vec3* const pRotation)
{
   if (!pRoot) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to create actor from null XML-element");
      return std::shared_ptr<Actor>();
   }

   // Создаем актера
   std::shared_ptr<Actor> pActor = std::shared_ptr<Actor>(new Actor(GetNextActorId()));
   if (!pActor->Init(pRoot)) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize actor from XML");
      return std::shared_ptr<Actor>();
   }

   // Загружаем компоненты
   for (tinyxml2::XMLElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      std::shared_ptr<ActorComponent> pComponent(CreateComponent(pActor, pNode));
      if (pComponent) {
         pActor->AddComponent(pComponent);
      } else {
         return std::shared_ptr<Actor>();
      }
   }

   // Немного хаков для изменения позиции перед тем, как ее считают другие компоненты (например физика)
   auto pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

   if (pPosition)
      pTransformComponent->SetPosition(*pPosition);

   if (pRotation)
      pTransformComponent->SetRotation(*pRotation);

   // Теперь можем инициализировать компоненты
   pActor->PostInit();
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
      pComponent.reset(creator());
   } else {
      Logger::WriteLog(Logger::LogType::ERROR, "Couldn’t find ActorComponent named " + name);
      return std::shared_ptr<ActorComponent>();
   }

   // Назначаем владельца компонента
   pComponent->SetOwner(pActor);

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
   for (tinyxml2::XMLElement* pNode = overrides->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      std::string name(pNode->Value());
      auto pComponent = pActor->GetComponent<ActorComponent>(name).lock();
      if (pComponent)
         pComponent->Init(pNode);
      else {
         pComponent = CreateComponent(pActor, pNode);
         if (pComponent) {
            pActor->AddComponent(pComponent);
            pComponent->PostInit();
         }
      }
   }
}
} // namespace BIEngine
