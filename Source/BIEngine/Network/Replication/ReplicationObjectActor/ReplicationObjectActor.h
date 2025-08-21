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

   virtual std::shared_ptr<Actor> ConstructReplicatedObject(bool isMaster) override;

private:
   String m_masterActorFilePath;
   String m_slaveActorFilePath;
};

} // namespace BIEngine
