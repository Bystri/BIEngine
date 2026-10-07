#pragma once

#include "../ReplicationUnit.h"
#include "../../Actors/TransformComponent.h"

namespace BIEngine {

class Process;

class TransformReplicationUnit : public ReplicationUnit<Actor> {

public:
   TransformReplicationUnit(bool shouldInterpoalte)
      : m_shouldInterpolate(shouldInterpoalte)
   {

   }

   virtual ReplicationUnitTypeId GetTypeId() const override { return 0x7a818985; }

   virtual void Init(ReplicationObject* pRelicationObject, SharedPtr<Actor> pActor) override;

   virtual bool IsStateChanged() override;

   virtual void Write(OutputMemoryBitStream& stream) override;
   virtual void Read(InputMemoryBitStream& stream) override;

protected:
   bool ShouldInterpolate() const { return m_shouldInterpolate; }

private:
   SharedPtr<TransformComponent> m_pTransformComponent;

   WeakPtr<Process> m_pInterpolationProcess;

   glm::vec3 m_cachedPosition = glm::vec3(0.0f);
   glm::vec3 m_cachedRotation = glm::vec3(0.0f);

   bool m_shouldInterpolate = false;
};

} // namespace BIEngine
