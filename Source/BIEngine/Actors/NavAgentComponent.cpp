#include "NavAgentComponent.h"

#include "../Actors/TransformComponent.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId NavAgentComponent::g_CompId = "NavAgentComponent";

NavAgentComponent::NavAgentComponent()
   : m_pNavCrowd(nullptr), m_agentId(-1)
{
}

bool NavAgentComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pCommonSettings = pData->FirstChildElement("Common");
   if (pCommonSettings) {
      float maxSpeed = 3.5f;

      pCommonSettings->QueryFloatAttribute("maxSpeed", &maxSpeed);
      m_params.MaxSpeed = maxSpeed;
   }

   return true;
}

void NavAgentComponent::Activate()
{
   if (!g_pApp->m_pGameLogic->GetNavWorld()) {
      return;
   }

   m_pNavCrowd = g_pApp->m_pGameLogic->GetNavWorld()->GetNavCrowd();

   SharedPtr<TransformComponent> pTransformComponent = GetOwner()->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
   m_agentId = m_pNavCrowd->AddAgent(GetOwner()->GetId(), pTransformComponent->GetPosition(), m_params);
}

void NavAgentComponent::Deactivate()
{
   if (m_agentId != -1) {
      m_pNavCrowd->RemoveAgent(m_agentId);
   }
}

tinyxml2::XMLElement* NavAgentComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());

   tinyxml2::XMLElement* pCommonSettings = pDoc->NewElement("Common");
   pCommonSettings->SetAttribute("maxSpeed", m_params.MaxSpeed);
   pBaseElement->LinkEndChild(pCommonSettings);

   return pBaseElement;
}

bool NavAgentComponent::SetDestination(const glm::vec3& pos)
{
   if (m_agentId == -1) {
      return false;
   }

   return m_pNavCrowd->SetDestination(m_agentId, pos);
}

glm::vec3 NavAgentComponent::GetVelocity() const
{
   if (m_agentId == -1) {
      return glm::vec3(0.0f);
   }

   return m_pNavCrowd->GetVelocity(m_agentId);
}

glm::vec3 NavAgentComponent::GetDesiredInput() const
{
   if (m_agentId == -1) {
      return glm::vec3(0.0f);
   }

   return m_pNavCrowd->GetDesiredInput(m_agentId);
}

} // namespace BIEngine
