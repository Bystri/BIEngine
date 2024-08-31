#include "ObjectReplication.h"

namespace BIEngine {

/***LinkingContext***/

uint32_t LinkingContext::GetNetworkId(std::shared_ptr<ReplicationObject> pGameObject, bool shouldCreateIfNotFound)
{
   auto it = m_gameObjectToNetworkIdMap.find(pGameObject);
   if (it != m_gameObjectToNetworkIdMap.end()) {
      return it->second;
   }

   if (shouldCreateIfNotFound) {
      uint32_t newNetworkId = m_nextNetworkId++;
      AddGameObject(pGameObject, newNetworkId);
      return newNetworkId;
   }


   return 0;
}

void LinkingContext::AddGameObject(std::shared_ptr<ReplicationObject> pGameObject, uint32_t networkId)
{
   m_networkIdToGameObjectMap[networkId] = pGameObject;
   m_gameObjectToNetworkIdMap[pGameObject] = networkId;
}

void LinkingContext::RemoveGameObject(std::shared_ptr<ReplicationObject> pGameObject)
{
   uint32_t networkId = m_gameObjectToNetworkIdMap[pGameObject];
   m_gameObjectToNetworkIdMap.erase(pGameObject);
   m_networkIdToGameObjectMap.erase(networkId);
}

std::shared_ptr<ReplicationObject> LinkingContext::GetGameObject(uint32_t networkId)
{
   auto it = m_networkIdToGameObjectMap.find(networkId);
   if (it != m_networkIdToGameObjectMap.end()) {
      return it->second;
   }

   return nullptr;
}

/***ReplicationHeader***/

void ReplicationHeader::Write(OutputMemoryBitStream& stream)
{
   stream.WriteBits(static_cast<uint32_t>(m_replicationAction), GetRequiredBits<static_cast<int>(ReplicationAction::MAX)>::Value);

   Serialize(stream, m_networkId);
   if (m_replicationAction != ReplicationAction::Destroy) {
      Serialize(stream, m_classId);
   }
}

void ReplicationHeader::Read(InputMemoryBitStream& stream)
{
   uint32_t repAct = 0;
   Deserialize(stream, repAct, GetRequiredBits<static_cast<int>(ReplicationAction::MAX)>::Value);
   m_replicationAction = static_cast<ReplicationAction>(repAct);

   Deserialize(stream, m_networkId);
   if (m_replicationAction != ReplicationAction::Destroy) {
      Deserialize(stream, m_classId);
   }
};

/***ReplicationManager***/

void ReplicationManager::ReplicateCreate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Create, m_pLinkingContext->GetNetworkId(pGameObject, true), pGameObject->GetClassId());
   rh.Write(stream);
   pGameObject->Write(stream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Update, m_pLinkingContext->GetNetworkId(pGameObject, false), pGameObject->GetClassId());
   rh.Write(stream);
   pGameObject->Write(stream);
}

void ReplicationManager::ReplicateDestroy(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Destroy, m_pLinkingContext->GetNetworkId(pGameObject, false));
   rh.Write(stream);
}

void ReplicationManager::ProcessReplicationAction(InputMemoryBitStream& stream)
{
   ReplicationHeader rh;
   rh.Read(stream);
   switch (rh.GetReplicationAction()) {
      case ReplicationAction::Create:
         {
            std::shared_ptr<ReplicationObject> go = ObjectCreationRegistry::Get().CreateGameObject(rh.GetClassId());
            m_pLinkingContext->AddGameObject(go, rh.GetNetworkId());
            go->Read(stream);
            break;
         }
      case ReplicationAction::Update:
         {
            std::shared_ptr<ReplicationObject> go = m_pLinkingContext->GetGameObject(rh.GetNetworkId());
            // we might have not received the create yet,
            // so serialize into a dummy to advance read head
            if (go) {
               go->Read(stream);
            } else {
               uint32_t classId = rh.GetClassId();
               go = ObjectCreationRegistry::Get().CreateGameObject(classId);
               go->Read(stream);
            }
            break;
         }
      case ReplicationAction::Destroy:
         {
            std::shared_ptr<ReplicationObject> go = m_pLinkingContext->GetGameObject(rh.GetNetworkId());
            m_pLinkingContext->RemoveGameObject(go);
            break;
         }
      default:
         // not handled by us
         break;
   }
}

/***RPCManager***/

void RPCManager::RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func)
{
   Assert(m_nameToRPCTable.find(id) == m_nameToRPCTable.end(), "RPC function with id %u already registered in RPCManager", id);
   m_nameToRPCTable[id] = func;
}

void RPCManager::ProcessRPC(InputMemoryBitStream& stream)
{
   uint32_t name;
   Deserialize(stream, name);
   m_nameToRPCTable[name](stream);
}
} // namespace BIEngine
