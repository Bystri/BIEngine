#pragma once

#include "../../BIEngine/Network/Replication/ReplicationUnit.h"
#include "../Combat/CombatStateComponent.h"

class CombatReplicationUnit : public BIEngine::ReplicationUnit<BIEngine::Actor> {
public:
   virtual void Init(BIEngine::SharedPtr<BIEngine::Actor> pObject) override;
   virtual bool IsStateChanged() override;

   virtual void Write(BIEngine::OutputMemoryBitStream& stream) override;
   virtual void Read(BIEngine::InputMemoryBitStream& stream) override;

private:
   BIEngine::SharedPtr<CombatStateComponent> m_pCombatStateComponent;

   bool m_cachedIsAttackInProgress = false;
};
