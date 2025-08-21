#include "Actor.h"

#include <iostream>

#include "../Utilities/Logger.h"
#include "../StdLib/String.h"
#include "../EngineCore/Assert.h"
#include "ActorFactory.h"

namespace BIEngine {

Actor::Actor(ActorId id)
   : m_id(id), m_bIsActivated(false), m_activateFlag(true), m_isLevelLoaded(false), m_pParent(nullptr)
{
}

Actor::~Actor()
{
   Logger::WriteLog(Logger::LogType::MESSAGE, "Actor: Destroying Actor " + ToString(m_id));
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

void Actor::AddChild(std::shared_ptr<Actor> pChild)
{
   pChild->m_pParent = this;
   m_children.PushBack(pChild);
}

bool Actor::RemoveChild(ActorId id)
{
   for (auto itr = m_children.Begin(); itr != m_children.End(); ++itr) {
      if ((*itr)->RemoveChild(id)) {
         return true;
      }

      if (id == (*itr)->GetId()) {
         (*itr)->m_pParent = nullptr;
         itr = m_children.Erase(itr);
         return true;
      }
   }

   return false;
}

void Actor::Activate()
{
   if (!m_activateFlag || m_bIsActivated) {
      return;
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->Activate();
   }

   for (auto& child : m_children) {
      child->Activate();
   }

   m_bIsActivated = true;
}

void Actor::Deactivate()
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto& child : m_children) {
      child->Deactivate();
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->Deactivate();
   }

   m_bIsActivated = false;
   m_activateFlag = false;
}

void Actor::SetActivate(bool value)
{
   m_activateFlag = value;
   if (!m_isLevelLoaded) {
      return;
   }

   if (m_activateFlag) {
      Activate();
   } else {
      Deactivate();
   }
}

void Actor::OnLevelLoaded()
{
   m_isLevelLoaded = true;

   for (auto& child : m_children) {
      child->OnLevelLoaded();
   }
}

void Actor::OnUpdate(const GameTimer& gt)
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->OnUpdate(gt);
   }

   for (auto& child : m_children) {
      child->OnUpdate(gt);
   }
}

void Actor::OnRenderObject(const GameTimer& gt)
{
   if (!m_bIsActivated) {
      return;
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->OnRenderObject(gt);
   }

   for (auto& child : m_children) {
      child->OnRenderObject(gt);
   }
}

void Actor::Destroy()
{
   for (auto& child : m_children) {
      child->Destroy();
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->Terminate();
   }

   m_components.Clear();
}

void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent)
{
   auto success = m_components.Insert(pComponent->GetComponentId(), pComponent);
   Assert(success.second, "Cant load component %s", pComponent->GetComponentId().CStr());
}

tinyxml2::XMLElement* Actor::ToXML(tinyxml2::XMLDocument* pDoc) const
{
   if (!pDoc) {
      return nullptr;
   }

   // Generate actor info
   tinyxml2::XMLElement* const pActorElement = pDoc->NewElement("Actor");
   pActorElement->SetAttribute("name", m_name.CStr());

   // Add components
   tinyxml2::XMLElement* pComponentsElement = pDoc->NewElement("Components");
   pActorElement->LinkEndChild(pComponentsElement);
   for (auto it = m_components.CBegin(); it != m_components.CEnd(); ++it) {
      auto pComponent = it->second;
      tinyxml2::XMLElement* const pComponentElement = pComponent->GenerateXml(pDoc);
      pComponentsElement->LinkEndChild(pComponentElement);
   }

   // Add children
   tinyxml2::XMLElement* pChildrenElement = pDoc->NewElement("Children");
   pActorElement->LinkEndChild(pChildrenElement);
   for (const auto& child : m_children) {
      tinyxml2::XMLElement* const childElement = child->ToXML(pDoc);
      pChildrenElement->LinkEndChild(childElement);
   }

   return pActorElement;
}

Actor* Actor::GetActorByPath(const String& path)
{
   if (path == ".") {
      return this;
   }

   if (path.Size() == 2 && path[0] == '.' && path[1] == '.') {
      return m_pParent;
   }

   if (path.Size() > 2 && path[0] == '.' && path[1] == '.' && path[2] == '/') {
      const String newPath = &path.CStr()[3];
      return m_pParent->GetActorByPath(newPath);
   }

   std::size_t delimPos = path.Find('/');
   if (delimPos == String::NPos) {
      for (const auto& child : m_children) {
         if (child->GetName() == path) {
            return child.get();
         }
      }

      return nullptr;
   }

   const String childName = path.Substr(0, delimPos);
   for (const auto& child : m_children) {
      if (child->GetName() == childName) {
         const String newPath = &path.CStr()[delimPos + 1];
         return child->GetActorByPath(newPath);
      }
   }

   return nullptr;
}

} // namespace BIEngine
