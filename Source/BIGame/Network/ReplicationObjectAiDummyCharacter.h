#pragma once

#include "../../BIEngine/Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"
#include "../../BIEngine/Network/Replication/ReplicationObjectActor/TransformReplicationUnit.h"
#include "LocomotionInfoReplicationUnit.h"
#include "CombatReplicationUnit.h"
#include "HealthStateReplicationUnit.h"

class ReplicationObjectAiDummyCharacter : public BIEngine::ReplicationObjectActor {
public:
   CLASS_IDENTIFICATION('AIDC', ReplicationObjectAiDummyCharacter)

   ReplicationObjectAiDummyCharacter()
      : BIEngine::ReplicationObjectActor(
           "actors/ai_dummy_character_server.xml",
           "actors/ai_dummy_character_client.xml",
           std::move(ReplicationUnitArray{BIEngine::MakeShared<BIEngine::TransformReplicationUnit>(), BIEngine::MakeShared<LocomotionInfoReplicationUnit>(), BIEngine::MakeShared<CombatReplicationUnit>(), BIEngine::MakeShared<HealthStateReplicationUnit>()}))
   {
   }
};
