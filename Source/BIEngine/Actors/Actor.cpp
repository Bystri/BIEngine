#include "Actor.h"

#include <iostream>

#include "../Utilities/Logger.h"
#include "../StdLib/String.h"
#include "../StdLib/Assert.h"
#include "ActorFactory.h"

namespace BIEngine {

Actor::Actor(ActorId id)
   : m_id(id), m_pParent(nullptr)
{
   m_flags.Set(IS_ACTIVE_FLAG_IDX, true);
}

Actor::~Actor()
{
   Logger::WriteLog(Logger::LogType::MESSAGE, "Actor: Destroying Actor " + ToString(m_id));
}

bool Actor::Init(tinyxml2::XMLElement* pData)
{
   if (!pData)
      return false;

   const char* actorName = nullptr;
   pData->QueryStringAttribute("name", &actorName);
   m_name = actorName;

   return true;
}

void Actor::AddChild(SharedPtr<Actor> pChild)
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
   if (!m_flags.Test(IS_ACTIVE_FLAG_IDX) || m_flags.Test(IS_ACTIVATED_FLAG_IDX)) {
      return;
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->Activate();
   }

   for (auto& child : m_children) {
      child->Activate();
   }

   m_flags.Set(IS_ACTIVATED_FLAG_IDX, true);
}

void Actor::Deactivate()
{
   if (!m_flags.Test(IS_ACTIVATED_FLAG_IDX)) {
      return;
   }

   for (auto& child : m_children) {
      child->Deactivate();
   }

   for (auto it = m_components.Begin(); it != m_components.End(); ++it) {
      it->second->Deactivate();
   }

   m_flags.Set(IS_ACTIVATED_FLAG_IDX, false);
   m_flags.Set(IS_ACTIVE_FLAG_IDX, false);
}

void Actor::SetActivate(bool value)
{
   m_flags.Set(IS_ACTIVE_FLAG_IDX, value);
   if (!m_flags.Test(IS_LEVEL_LOADED_FLAG_IDX)) {
      return;
   }

   if (m_flags.Test(IS_ACTIVE_FLAG_IDX)) {
      Activate();
   } else {
      Deactivate();
   }
}

void Actor::OnLevelLoaded()
{
   m_flags.Set(IS_LEVEL_LOADED_FLAG_IDX, true);

   for (auto& child : m_children) {
      child->OnLevelLoaded();
   }
}

void Actor::OnUpdate(const GameTimer& gt)
{
   if (!m_flags.Test(IS_ACTIVATED_FLAG_IDX)) {
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
   if (!m_flags.Test(IS_ACTIVATED_FLAG_IDX)) {
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
      it->second->Deactivate();
      it->second->Terminate();
   }

   m_components.Clear();
}

void Actor::AddComponent(SharedPtr<ActorComponent> pComponent)
{
   auto success = m_components.Insert(pComponent->GetComponentId(), pComponent);
   Assert(success.second, "Can't load component %s", pComponent->GetComponentId().CStr());
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
            return child.Get();
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
