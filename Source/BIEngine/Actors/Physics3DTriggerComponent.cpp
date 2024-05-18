
#include "Physics3DTriggerComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics3D.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId Physics3DTriggerComponent::g_CompId = "Physics3DTriggerComponent";

Physics3DTriggerComponent::Physics3DTriggerComponent()
{
   m_dimension = glm::vec3(1.f, 1.f, 1.f);
}

bool Physics3DTriggerComponent::Init(tinyxml2::XMLElement* pData)
{
   m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics3D();
   if (!m_gamePhysics) {
      return false;
   }

   tinyxml2::XMLElement* pScaleElement = pData->FirstChildElement("Scale");
   if (pScaleElement) {
      double w = 0;
      double h = 0;
      double d = 0;
      pScaleElement->QueryDoubleAttribute("w", &w);
      pScaleElement->QueryDoubleAttribute("h", &h);
      pScaleElement->QueryDoubleAttribute("d", &d);
      m_dimension = glm::vec3(w, h, d);
   }

   return true;
}

void Physics3DTriggerComponent::Activate()
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   m_gamePhysics->CreateTrigger(m_pOwner->GetId(), pTransformComponent->GetPosition(), m_dimension);
}

void Physics3DTriggerComponent::Deactivate()
{
   m_gamePhysics->RemoveActor(m_pOwner->GetId());
}

tinyxml2::XMLElement* Physics3DTriggerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
   pScale->SetAttribute("w", std::to_string(m_dimension.x).c_str());
   pScale->SetAttribute("h", std::to_string(m_dimension.y).c_str());
   pScale->SetAttribute("d", std::to_string(m_dimension.z).c_str());
   pBaseElement->LinkEndChild(pScale);

   return pBaseElement;
}

glm::vec3 Physics3DTriggerComponent::GetVelocity()
{
   return m_gamePhysics->GetVelocity(m_pOwner->GetId());
}

void Physics3DTriggerComponent::SetVelocity(const glm::vec3& velocity)
{
   m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
}

void Physics3DTriggerComponent::RotateY(float angle)
{
   m_gamePhysics->RotateY(m_pOwner->GetId(), angle);
}

void Physics3DTriggerComponent::SetPosition(const glm::vec3& position)
{
   m_gamePhysics->SetPosition(m_pOwner->GetId(), position);
}

void Physics3DTriggerComponent::Stop()
{
   return m_gamePhysics->StopActor(m_pOwner->GetId());
}

} // namespace BIEngine
