#pragma once

#include "../../BIEngine/Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"
#include "../../BIEngine/Network/Replication/ReplicationObjectActor/TransformReplicationUnit.h"

class ReplicationObjectPlayerCharacter : public BIEngine::ReplicationObjectActor {
public:
   CLASS_IDENTIFICATION('PLCH', ReplicationObjectPlayerCharacter)

   ReplicationObjectPlayerCharacter()
      : BIEngine::ReplicationObjectActor(
           "actors/player_character_server.xml",
           "actors/player_character_client.xml",
           std::move(ReplicationUnitArray{std::make_shared<BIEngine::TransformReplicationUnit>()}))
   {
   }
};
