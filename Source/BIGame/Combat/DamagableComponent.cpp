#include "DamagableComponent.h"

#include "../../BIEngine/StdLib/Assert.h"
#include "../../BIEngine/Utilities/Logger.h"
#include "../../BIEngine/Actors/Actor.h"

const BIEngine::ComponentId DamagableComponent::g_CompId = "DamagableComponent";

bool DamagableComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to Locomotion Info Component");
   return true;
}

tinyxml2::XMLElement* DamagableComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void DamagableComponent::TakeDamage(float damage)
{
   if (m_isDead) {
      return;
   }

   m_health -= damage;

   BIEngine::Logger::WriteMsgLog("%s: Got %f damage! Remaining: %f.", GetOwner()->GetName().CStr(), damage, m_health);

   if (m_health <= 0.0f) {
      m_health = 0.0f;
      m_isDead = true;

      BIEngine::Logger::WriteMsgLog("%s: Is dead!", GetOwner()->GetName().CStr());
   }
}
