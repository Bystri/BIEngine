#pragma once

#include "../../BIEngine/Network/Replication/ReplicationUnit.h"
#include "../PlayerManager/PlayerManager.h"

class AttachedActorReplicationUnit : public BIEngine::ReplicationUnit<Player> {
public:
   virtual void Init(std::shared_ptr<Player> pObject) override;
   virtual bool IsStateChanged() override;

   virtual void Write(BIEngine::OutputMemoryBitStream& stream) override;
   virtual void Read(BIEngine::InputMemoryBitStream& stream) override;

private:
   std::shared_ptr<Player> m_pPlayer;
   BIEngine::ActorId m_cachedActorId = BIEngine::Actor::INVALID_ACTOR_ID;
};
