#pragma once

#include "../../BIEngine/Network/Replication/ReplicationUnit.h"
#include "../Locomotion/LocomotionInfoComponent.h"

class LocomotionInfoReplicationUnit : public BIEngine::ReplicationUnit<BIEngine::Actor> {
public:
   virtual void Init(BIEngine::ReplicationObject* pRelicationObject, BIEngine::SharedPtr<BIEngine::Actor> pObject) override;
   virtual bool IsStateChanged() override;

   virtual void Write(BIEngine::OutputMemoryBitStream& stream) override;
   virtual void Read(BIEngine::InputMemoryBitStream& stream) override;

private:
   BIEngine::SharedPtr<LocomotionInfoComponent> m_pLocomotionInfoComponent;

   glm::vec3 m_cachedCurVel = glm::vec3(0.0f);
   glm::vec2 m_cachedCurDir = glm::vec2(0.0f);
   glm::vec3 m_cachedDesiredVel = glm::vec3(0.0f);
   glm::vec2 m_cachedDesiredDir = glm::vec2(0.0f);
};
