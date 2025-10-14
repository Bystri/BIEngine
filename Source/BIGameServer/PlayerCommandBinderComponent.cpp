#include "PlayerCommandBinderComponent.h"

#include "../BIGame/BIEventListener.h"
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
   m_onCommandMoveToHandler = BIEngine::EventManager::Get()->AddListener(
      MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(PlayerCommandBinderComponent::HandleOnCommandMoveTo),
      EvtData_PlayerCommandMoveTo::sk_EventType);
}

void PlayerCommandBinderComponent::Deactivate()
{
   BIEngine::EventManager::Get()->RemoveListener(m_onCommandMoveToHandler);
}

void PlayerCommandBinderComponent::HandleOnCommandMoveTo(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_PlayerCommandMoveTo> pCastEventData = BIEngine::StaticPointerCast<EvtData_PlayerCommandMoveTo>(pEventData);

   const uint32_t playerId = GetOwner()->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock()->GetPlayerId();
   if (playerId != pCastEventData->GetPlayerId()) {
      return;
   }

   BIEngine::Logger::WriteMsgLog("player %d got move to command to %f %f %f", playerId, pCastEventData->GetPosToMove().x, pCastEventData->GetPosToMove().y, pCastEventData->GetPosToMove().z);
}
