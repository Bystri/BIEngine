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
   void DestroyPlayer(PlayerId id);

   void SetLocalPlayer(PlayerId id)
   {
      m_localPlayer = id;
   }

   PlayerId GetLocalPlayerId() const
   {
      return m_localPlayer;
   }

private:
   PlayerManager() = default;

private:
   static PlayerManager* s_pSingleton;
   PlayerId m_localPlayer = INVALID_PLAYER_ID;
   PlayerId m_nextPlayerId = 1;
   BIEngine::HashMap<PlayerId, BIEngine::SharedPtr<Player>> m_players;
};
