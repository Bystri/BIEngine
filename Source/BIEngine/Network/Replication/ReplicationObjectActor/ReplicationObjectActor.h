#pragma once

#include "../ObjectReplication.h"
#include "../../Actors/Actor.h"

namespace BIEngine {

class ReplicationObjectActor : public ReplicationObjectT<Actor> {
public:
   ReplicationObjectActor(
      const std::string& masterActorFilePath,
      const std::string& slaveActorFilePath,
      ReplicationUnitArray replicationUnits)
      : ReplicationObjectT<Actor>(std::move(replicationUnits)),
        m_masterActorFilePath(masterActorFilePath),
        m_slaveActorFilePath(slaveActorFilePath)
   {
   }

   virtual std::shared_ptr<Actor> ConstructReplicatedObject(bool isMaster) override;

private:
   std::string m_masterActorFilePath;
   std::string m_slaveActorFilePath;
};

} // namespace BIEngine
