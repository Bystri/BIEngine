#pragma once

#include "../../BIEngine/Network/Replication/ReplicationUnit.h"
#include "../Combat/HealthStateComponent.h"

class HealthStateReplicationUnit : public BIEngine::ReplicationUnit<BIEngine::Actor> {
public:
   virtual void Init(BIEngine::ReplicationObject* pRelicationObject, BIEngine::SharedPtr<BIEngine::Actor> pObject) override;
   virtual bool IsStateChanged() override;

   virtual void Write(BIEngine::OutputMemoryBitStream& stream) override;
   virtual void Read(BIEngine::InputMemoryBitStream& stream) override;

private:
   BIEngine::SharedPtr<HealthStateComponent> m_pHealthStateComponent;

   float m_cachedHealth = 0.0;
   bool m_cachedIsDead = false;
};
