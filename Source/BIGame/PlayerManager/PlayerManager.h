#pragma once

#include <vector>
#include <memory>

#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/EngineCore/Assert.h"
#include "../../BIEngine/Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"

using PlayerId = uint32_t;

class Player {
public:
   Player(PlayerId id)
      : m_id(id)
   {
   }

   PlayerId GetId() const { return m_id; }

   void SetPlayableActor(std::shared_ptr<BIEngine::Actor> pActor);

   std::shared_ptr<BIEngine::Actor> GetPlayableActor() const { return m_pPlayableActor; }

private:
   PlayerId m_id;

   std::shared_ptr<BIEngine::Actor> m_pPlayableActor;
};

class PlayerManager {
public:
   static constexpr PlayerId INVALID_PLAYER_ID = -1;

   static bool Create();
   static void Destroy();

   static PlayerManager* Get()
   {
      BIEngine::Assert(s_pSingleton, "Call PlayerManager::Create before PlayerManager::Get");
      return s_pSingleton;
   }

   std::shared_ptr<Player> CreatePlayer();

private:
   PlayerManager() = default;

private:
   static PlayerManager* s_pSingleton;
   PlayerId m_nextPlayerId = 0;
};
