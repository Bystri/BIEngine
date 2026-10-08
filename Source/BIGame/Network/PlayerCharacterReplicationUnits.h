#pragma once

#include "../../BIEngine/Network/Replication/ReplicationObjectActor/TransformReplicationUnit.h"
#include "LocomotionInfoReplicationUnit.h"
#include "EventNetworkProtocol.h"
#include "../CharacterMovementComponent.h"

// The owning client restores its complete movement state from EventNetworkProtocol.
// Ordinary replication is still used by clients observing another player.
class PlayerTransformReplicationUnit : public BIEngine::TransformReplicationUnit {
public:
   PlayerTransformReplicationUnit() : TransformReplicationUnit(false) {}

   virtual void Init(BIEngine::ReplicationObject* owner, BIEngine::SharedPtr<BIEngine::Actor> actor) override
   {
      TransformReplicationUnit::Init(owner, actor);
      m_actor = actor;
   }

   virtual void Read(BIEngine::InputMemoryBitStream& stream) override
   {
      auto* leader = EventProtocolLeader::Get();
      if (!leader || !leader->IsLocalPlayerActor(m_actor.Get())) {
         TransformReplicationUnit::Read(stream);
         auto movement = m_actor->GetComponent<CharacterMovementComponent>(CharacterMovementComponent::g_CompId).Lock();
         if (movement) {
            movement->OnRemoteSnapshotReceived();
         }
         return;
      }

      float ignored;
      for (int i = 0; i < 6; ++i) {
         BIEngine::Deserialize(stream, ignored);
      }
   }

private:
   BIEngine::SharedPtr<BIEngine::Actor> m_actor;
};

class PlayerLocomotionReplicationUnit : public LocomotionInfoReplicationUnit {
public:
   virtual void Init(BIEngine::ReplicationObject* owner, BIEngine::SharedPtr<BIEngine::Actor> actor) override
   {
      LocomotionInfoReplicationUnit::Init(owner, actor);
      m_actor = actor;
   }

   virtual void Read(BIEngine::InputMemoryBitStream& stream) override
   {
      auto* leader = EventProtocolLeader::Get();
      if (!leader || !leader->IsLocalPlayerActor(m_actor.Get())) {
         LocomotionInfoReplicationUnit::Read(stream);
         auto movement = m_actor->GetComponent<CharacterMovementComponent>(CharacterMovementComponent::g_CompId).Lock();
         if (movement) {
            movement->OnRemoteSnapshotReceived();
         }
         return;
      }

      glm::vec3 ignored3;
      glm::vec2 ignored2;
      float ignored;
      stream.ReadBytes(&ignored3, sizeof(ignored3));
      stream.ReadBytes(&ignored2, sizeof(ignored2));
      stream.ReadBytes(&ignored3, sizeof(ignored3));
      stream.ReadBytes(&ignored2, sizeof(ignored2));
      stream.ReadBytes(&ignored, sizeof(ignored));
      stream.ReadBytes(&ignored, sizeof(ignored));
   }

private:
   BIEngine::SharedPtr<BIEngine::Actor> m_actor;
};
