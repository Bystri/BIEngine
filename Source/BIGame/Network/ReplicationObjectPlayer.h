#pragma once

#include "../../BIEngine/Network/Replication/ObjectReplication.h"

#include "../PlayerManager/PlayerManager.h"
#include "AttachedActorReplicationUnit.h"

class ReplicationObjectPlayer : public BIEngine::ReplicationObjectT<Player> {
public:
   CLASS_IDENTIFICATION('PLYR', ReplicationObjectPlayer)

   ReplicationObjectPlayer()
      : ReplicationObjectT<Player>(std::move(ReplicationUnitArray{std::make_shared<AttachedActorReplicationUnit>()}))
   {
   }

   virtual std::shared_ptr<Player> ConstructReplicatedObject(bool isMaster) override
   {
      return PlayerManager::Get()->CreatePlayer();
   }

private:
   std::shared_ptr<Player> m_pPlayer;

   bool m_isDirty = false;
   BIEngine::ActorId m_cachedActorId = BIEngine::Actor::INVALID_ACTOR_ID;
};
