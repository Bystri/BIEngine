#include "NavAgentComponent.h"

#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId NavAgentComponent::g_CompId = "NavAgentComponent";

NavAgentComponent::NavAgentComponent()
   : m_pNavCrowd(nullptr), m_agentId(-1)
{
}

NavAgentComponent::~NavAgentComponent()
{
   if (m_agentId != -1) {
      m_pNavCrowd->RemoveAgent(m_agentId);
   }
}

bool NavAgentComponent::Init(tinyxml2::XMLElement* pData)
{
   return true;
}

void NavAgentComponent::PostInit()
{
   if (!g_pApp->m_pGameLogic->GetNavWorld()) {
      return;
   }

   m_pNavCrowd = g_pApp->m_pGameLogic->GetNavWorld()->GetNavCrowd();

   m_agentId = m_pNavCrowd->AddAgent(m_pOwner);
}

tinyxml2::XMLElement* NavAgentComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

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

} // namespace BIEngine
