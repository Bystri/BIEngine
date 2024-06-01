#include "Actor.h"

#include <iostream>
#include <string>

#include "../Utilities/Logger.h"
#include "../EngineCore/Assert.h"
#include "ActorFactory.h"

namespace BIEngine {

Actor::Actor(ActorId id)
   : m_id(id), m_bIsActivated(false)
{
}

Actor::~Actor()
{
   Logger::WriteLog(Logger::LogType::MESSAGE, "Actor: Destroying Actor " + std::to_string(m_id));
}

bool Actor::Init(tinyxml2::XMLElement* pData)
{
   if (!pData)
      return false;

   const char* actorName;
   pData->QueryStringAttribute("name", &actorName);
   m_name = actorName;

   return true;
}

void Actor::Activate()
{
   if (m_bIsActivated) {
      return;
   }

   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      it->second->Activate();
   }

   m_bIsActivated = true;
}

void Actor::Deactivate()
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      it->second->Deactivate();
   }

   m_bIsActivated = false;
}

void Actor::OnUpdate(const GameTimer& gt)
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      it->second->OnUpdate(gt);
   }
}

void Actor::OnRenderObject(const GameTimer& gt)
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      it->second->OnRenderObject(gt);
   }
}

void Actor::Destroy()
{
   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      it->second->Terminate();
   }
   m_components.clear();
}

void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent)
{
   std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->GetComponentId(), pComponent));
   Assert(success.second, "Cant load component %s", pComponent->GetComponentId().c_str());
}

tinyxml2::XMLElement* Actor::ToXML(tinyxml2::XMLDocument* pDoc) const
{
   if (!pDoc)
      return nullptr;

   // Создание и присоединение основного корня XML-элемента
   tinyxml2::XMLElement* pActorElement = pDoc->NewElement("Actor");
   pActorElement->SetAttribute("name", m_name.c_str());
   pDoc->LinkEndChild(pActorElement);

   // Создание и присоединение XML-элементов компонентов
   for (auto it = m_components.begin(); it != m_components.end(); ++it) {
      auto pComponent = it->second;
      tinyxml2::XMLElement* pComponentElement = pComponent->GenerateXml(pDoc);
      pActorElement->LinkEndChild(pComponentElement);
   }

   return pActorElement;
}

} // namespace BIEngine
