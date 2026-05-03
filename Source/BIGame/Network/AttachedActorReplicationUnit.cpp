#include "AttachedActorReplicationUnit.h"

#include "../../BIEngine/EngineCore/GameApp.h"
#include "../../BIEngine/Actors/NetworkReplicatedComponent.h"
#include "../../BIEngine/Network/Replication/ObjectReplicationProtocol.h"

void AttachedActorReplicationUnit::Init(BIEngine::ReplicationObject* pRelicationObject, BIEngine::SharedPtr<Player> pObject)
{
   ReplicationUnit::Init(pRelicationObject, pObject);
   m_pPlayer = pObject;
}

bool AttachedActorReplicationUnit::IsStateChanged()
{
   BIEngine::ActorId curActorId = BIEngine::Actor::INVALID_ACTOR_ID;
   if (m_pPlayer->GetPlayableActor() != nullptr) {
      curActorId = m_pPlayer->GetPlayableActor()->GetId();
   }

   const bool ret = curActorId != m_cachedActorId;
   m_cachedActorId = curActorId;

   return ret;
}

void AttachedActorReplicationUnit::Write(BIEngine::OutputMemoryBitStream& stream)
{
   auto pReplicationComponent = m_pPlayer->GetPlayableActor()->GetComponent<BIEngine::NetworkReplicatedComponent>(BIEngine::NetworkReplicatedComponent::g_CompId);
   BIEngine::Serialize(stream, pReplicationComponent.Lock()->GetReplicationObject()->GetNetworkId());
}

void AttachedActorReplicationUnit::Read(BIEngine::InputMemoryBitStream& stream)
{
   uint32_t networkId;
   BIEngine::Deserialize(stream, networkId);
   BIEngine::SharedPtr<BIEngine::ReplicationObjectActor> pObject = BIEngine::StaticPointerCast<BIEngine::ReplicationObjectActor>(BIEngine::ObjectReplicationProtocolReader::Get()->GetReplicationObject(networkId));
   if (pObject == nullptr) {
      BIEngine::Logger::WriteErrorLog("Attempt to add unexisted replicated actor [NetworkID:%u]", networkId);
      return;
   }

   m_pPlayer->SetPlayableActor(pObject->GetReplicatedObject());
}
