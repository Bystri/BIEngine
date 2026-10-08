#pragma once

#include "../../BIEngine/Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"
#include "PlayerCharacterReplicationUnits.h"
#include "CombatReplicationUnit.h"

class ReplicationObjectPlayerCharacter : public BIEngine::ReplicationObjectActor {
public:
   CLASS_IDENTIFICATION('PLCH', ReplicationObjectPlayerCharacter)

   ReplicationObjectPlayerCharacter()
      : BIEngine::ReplicationObjectActor(
           "actors/player_character_server.xml",
           "actors/player_character_client.xml",
           std::move(ReplicationUnitArray{BIEngine::MakeShared<PlayerTransformReplicationUnit>(), BIEngine::MakeShared<PlayerLocomotionReplicationUnit>(), BIEngine::MakeShared<CombatReplicationUnit>()}))
   {
   }
};
