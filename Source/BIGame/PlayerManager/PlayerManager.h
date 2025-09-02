#pragma once

#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/StdLib/SharedPtr.h"
#include "../../BIEngine/Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"

using PlayerId = uint32_t;

class Player {
public:
   Player(PlayerId id)
      : m_id(id)
   {
   }

   PlayerId GetId() const { return m_id; }

   void SetPlayableActor(BIEngine::SharedPtr<BIEngine::Actor> pActor);

   BIEngine::SharedPtr<BIEngine::Actor> GetPlayableActor() const { return m_pPlayableActor; }

private:
   PlayerId m_id;

   BIEngine::SharedPtr<BIEngine::Actor> m_pPlayableActor;
};

class PlayerManager {
public:
   static constexpr PlayerId INVALID_PLAYER_ID = -1;

   static bool Create();
   static void Destroy();

   static PlayerManager* Get();

   BIEngine::SharedPtr<Player> CreatePlayer();

private:
   PlayerManager() = default;

private:
   static PlayerManager* s_pSingleton;
   PlayerId m_nextPlayerId = 0;
};
