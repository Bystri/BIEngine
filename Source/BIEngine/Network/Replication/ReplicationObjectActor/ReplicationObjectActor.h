#pragma once

#include "../ObjectReplication.h"
#include "../../Actors/Actor.h"

namespace BIEngine {

class ReplicationObjectActor : public ReplicationObjectT<Actor> {
public:
   ReplicationObjectActor(
      const String& masterActorFilePath,
      const String& slaveActorFilePath,
      ReplicationUnitArray replicationUnits)
      : ReplicationObjectT<Actor>(std::move(replicationUnits)),
        m_masterActorFilePath(masterActorFilePath),
        m_slaveActorFilePath(slaveActorFilePath)
   {
   }

   virtual SharedPtr<Actor> ConstructReplicatedObject(bool isMaster) override;
   virtual void DestructReplicationObject(bool isMaster) override;

   virtual bool IsUseRelevancy() const override { return true; }

   virtual glm::vec3 GetPosition() const override;

private:
   String m_masterActorFilePath;
   String m_slaveActorFilePath;
};

} // namespace BIEngine
