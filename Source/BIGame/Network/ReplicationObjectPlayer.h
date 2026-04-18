#pragma once

#include "../../BIEngine/Network/Replication/ObjectReplication.h"

#include "../PlayerManager/PlayerManager.h"
#include "AttachedActorReplicationUnit.h"

class ReplicationObjectPlayer : public BIEngine::ReplicationObjectT<Player> {
public:
   CLASS_IDENTIFICATION('PLYR', ReplicationObjectPlayer)

   ReplicationObjectPlayer()
      : ReplicationObjectT<Player>(std::move(ReplicationUnitArray{BIEngine::MakeShared<AttachedActorReplicationUnit>()}))
   {
   }

   virtual BIEngine::SharedPtr<Player> ConstructReplicatedObject(bool isMaster) override
   {
      return PlayerManager::Get()->CreatePlayer();
   }

   virtual void DestructReplicationObject(bool isMaster) override
   {
      PlayerManager::Get()->DestroyPlayer(GetReplicatedObject()->GetId());
   }

private:
   bool m_isDirty = false;
   BIEngine::ActorId m_cachedActorId = BIEngine::Actor::INVALID_ACTOR_ID;
};
