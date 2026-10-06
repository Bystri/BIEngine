#include "PlayerCommandBinderComponent.h"

#include "../BIGame/BIEventListener.h"
#include "../BIGame/Locomotion/LocomotionInfoComponent.h"
#include "../BIEngine/Actors/NavAgentComponent.h"
#include "../BIEngine/Actors/PlayerComponent.h"

const BIEngine::ComponentId PlayerCommandBinderComponent::g_CompId = "PlayerCommandBinderComponent";

bool PlayerCommandBinderComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to PlayerCommandBinderComponent");
   return true;
}

tinyxml2::XMLElement* PlayerCommandBinderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void PlayerCommandBinderComponent::Activate()
{
    m_onCommandCharacterInput = BIEngine::EventManager::Get()->AddListener(
      MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(PlayerCommandBinderComponent::HandleOnCommandCharacterInput),
       EvtData_CharacterInput::sk_EventType);
}

void PlayerCommandBinderComponent::Deactivate()
{
   BIEngine::EventManager::Get()->RemoveListener(m_onCommandCharacterInput);
}

void PlayerCommandBinderComponent::HandleOnCommandCharacterInput(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_CharacterInput> pCastEventData = BIEngine::StaticPointerCast<EvtData_CharacterInput>(pEventData);

   const uint32_t playerId = GetOwner()->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock()->GetPlayerId();
   if (playerId != pCastEventData->GetPlayerId()) {
      return;
   }

   const glm::vec3 desiredVel = glm::vec3(pCastEventData->GetDesiredHorizontalAmount(), 0.0f, pCastEventData->GetDesiredVerticalAmount());
   const float desiredVelLength = glm::length(desiredVel);
   const glm::vec2 desiredDir = glm::normalize(glm::vec2(desiredVel.x, desiredVel.z));

   auto pLocomotionInfoComponent = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   pLocomotionInfoComponent->SetInputVel(desiredVel);
   pLocomotionInfoComponent->SetInputDir(pCastEventData->GetDesiredDir());
}