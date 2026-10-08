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
   while (!m_pendingCharacterInputs.Empty()) {
      m_pendingCharacterInputs.Pop();
   }
}

void PlayerCommandBinderComponent::HandleOnCommandCharacterInput(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_CharacterInput> pCastEventData = BIEngine::StaticPointerCast<EvtData_CharacterInput>(pEventData);

   const uint32_t playerId = GetOwner()->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock()->GetPlayerId();
   if (playerId != pCastEventData->GetPlayerId()) {
      return;
   }

   m_pendingCharacterInputs.Push(pCastEventData);
}

bool PlayerCommandBinderComponent::ApplyNextCharacterInput()
{
   if (m_pendingCharacterInputs.Empty()) {
      return false;
   }

   auto input = m_pendingCharacterInputs.Front();
   m_pendingCharacterInputs.Pop();
   m_appliedInputSequence = input->GetSequence();

   auto pLocomotionInfoComponent = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   pLocomotionInfoComponent->SetInputVel(glm::vec3(input->GetDesiredHorizontalAmount(), 0.0f, input->GetDesiredVerticalAmount()));
   pLocomotionInfoComponent->SetInputDir(input->GetDesiredDir());
   return true;
}

void PlayerCommandBinderComponent::MarkCharacterInputProcessed()
{
   m_lastProcessedInputSequence = m_appliedInputSequence;
}
