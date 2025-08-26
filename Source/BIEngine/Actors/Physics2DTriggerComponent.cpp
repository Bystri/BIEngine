
#include "Physics2DTriggerComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics3D.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId Physics2DTriggerComponent::g_CompId = "Physics2DTriggerComponent";

Physics2DTriggerComponent::Physics2DTriggerComponent()
{
   m_dimension = glm::vec2(1.f, 1.f);
}

bool Physics2DTriggerComponent::Init(tinyxml2::XMLElement* pData)
{
   m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics2D();
   if (!m_gamePhysics) {
      return false;
   }

   tinyxml2::XMLElement* pScaleElement = pData->FirstChildElement("Scale");
   if (pScaleElement) {
      double w = 0;
      double h = 0;
      pScaleElement->QueryDoubleAttribute("w", &w);
      pScaleElement->QueryDoubleAttribute("h", &h);
      m_dimension = glm::vec2(w, h);
   }

   return true;
}

void Physics2DTriggerComponent::Activate()
{
   SharedPtr<TransformComponent> pTransformComponent = GetOwner()->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
   m_gamePhysics->CreateTrigger(GetOwner()->GetId(), pTransformComponent->GetPosition(), m_dimension);
}

void Physics2DTriggerComponent::Deactivate()
{
   m_gamePhysics->RemoveActor(GetOwner()->GetId());
}

tinyxml2::XMLElement* Physics2DTriggerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());

   tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
   pScale->SetAttribute("w", ToString(m_dimension.x).CStr());
   pScale->SetAttribute("h", ToString(m_dimension.y).CStr());
   pBaseElement->LinkEndChild(pScale);

   return pBaseElement;
}

glm::vec2 Physics2DTriggerComponent::GetVelocity()
{
   return m_gamePhysics->GetVelocity(GetOwner()->GetId());
}

void Physics2DTriggerComponent::SetVelocity(const glm::vec2& velocity)
{
   m_gamePhysics->SetVelocity(GetOwner()->GetId(), velocity);
}

void Physics2DTriggerComponent::Rotate(float angle)
{
   m_gamePhysics->Rotate(GetOwner()->GetId(), angle);
}

void Physics2DTriggerComponent::SetPosition(const glm::vec2& position)
{
   m_gamePhysics->SetPosition(GetOwner()->GetId(), position);
}

void Physics2DTriggerComponent::Stop()
{
   return m_gamePhysics->StopActor(GetOwner()->GetId());
}

} // namespace BIEngine
