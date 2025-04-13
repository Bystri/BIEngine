#pragma once

#include "../ReplicationUnit.h"
#include "../../Actors/TransformComponent.h"

namespace BIEngine {

class TransformReplicationUnit : public ReplicationUnit<Actor> {
public:
   virtual void Init(std::shared_ptr<Actor> pActor) override;

   virtual bool IsStateChanged() override;

   virtual void Write(OutputMemoryBitStream& stream) override;
   virtual void Read(InputMemoryBitStream& stream) override;

private:
   std::shared_ptr<TransformComponent> m_pTransformComponent;

   glm::vec3 m_cachedPosition = glm::vec3(0.0f);
   glm::vec3 m_cachedRotation = glm::vec3(0.0f);
};

} // namespace BIEngine
