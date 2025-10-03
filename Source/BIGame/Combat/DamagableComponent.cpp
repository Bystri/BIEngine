#include "DamagableComponent.h"

#include "../../BIEngine/StdLib/Assert.h"
#include "../../BIEngine/Utilities/Logger.h"
#include "../../BIEngine/Actors/Actor.h"
#include "HealthStateComponent.h"

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
   BIEngine::SharedPtr<HealthStateComponent> pHealthState = GetOwner()->GetComponent<HealthStateComponent>(HealthStateComponent::g_CompId).Lock();
   if (pHealthState->IsDead()) {
      return;
   }

   float health = pHealthState->GetHealth();
   health -= damage;

   BIEngine::Logger::WriteMsgLog("%s: Got %f damage! Remaining: %f.", GetOwner()->GetName().CStr(), damage, health);

   if (health <= 0.0f) {
      health = 0.0f;
      pHealthState->SetIsDead(true);
   }

   pHealthState->SetHealth(health);
}
