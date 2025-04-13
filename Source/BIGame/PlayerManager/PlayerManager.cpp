#include "PlayerManager.h"

#include "../../BIEngine/Utilities/Logger.h"
#include "../../BIEngine/Actors/PlayerComponent.h"
#include "../BIEventListener.h"

PlayerManager* PlayerManager::s_pSingleton = nullptr;

void Player::SetPlayableActor(std::shared_ptr<BIEngine::Actor> pActor)
{
   if (m_pPlayableActor == pActor) {
      return;
   }

   m_pPlayableActor = pActor;

   if (m_pPlayableActor == nullptr) {
       //TODO: Player actor detached; Fire event
      return;
   }

   m_pPlayableActor->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).lock()->SetPlayerId(m_id);

   std::shared_ptr<EvtData_PlayerActor_Created> pEvent = std::make_shared<EvtData_PlayerActor_Created>(m_id, m_pPlayableActor->GetId());
   BIEngine::EventManager::Get()->QueueEvent(pEvent);
}

bool PlayerManager::Create()
{
   if (s_pSingleton) {
      BIEngine::Logger::WriteErrorLog("Overwriting PlayerManager singleton");
      if (s_pSingleton) {
         delete s_pSingleton;
         s_pSingleton = nullptr;
      }
   }

   s_pSingleton = new PlayerManager();
   if (s_pSingleton)
      return true;

   return false;
}

void PlayerManager::Destroy()
{
   if (s_pSingleton) {
      delete s_pSingleton;
      s_pSingleton = nullptr;
   }
}

std::shared_ptr<Player> PlayerManager::CreatePlayer()
{
   auto ret = std::make_shared<Player>(m_nextPlayerId++);

   std::shared_ptr<EvtData_Player_Created> pEvent = std::make_shared<EvtData_Player_Created>(ret);
   BIEngine::EventManager::Get()->QueueEvent(pEvent);

   return ret;
}
