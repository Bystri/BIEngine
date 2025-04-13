#include "ObjectReplicationManagerMaster.h"

#include "../../Utilities/Logger.h"

namespace BIEngine {

static ObjectReplicationManagerMaster* g_pObjectReplicationManagerMaster = nullptr;

ObjectReplicationManagerMaster* ObjectReplicationManagerMaster::Get()
{
   if (!g_pObjectReplicationManagerMaster) {
      g_pObjectReplicationManagerMaster = new ObjectReplicationManagerMaster();
   }

   return g_pObjectReplicationManagerMaster;
}

ObjectReplicationManagerMaster::ObjectReplicationManagerMaster()
   : m_pLinkingContext(std::make_shared<NewtworkObjectLinkingContexts>())
{
   if (g_pObjectReplicationManagerMaster) {
      Logger::WriteErrorLog("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.\n");
      delete g_pObjectReplicationManagerMaster;
   }

   g_pObjectReplicationManagerMaster = this;
}

ObjectReplicationManagerMaster::~ObjectReplicationManagerMaster()
{
   if (g_pObjectReplicationManagerMaster == this) {
      delete g_pObjectReplicationManagerMaster;
      g_pObjectReplicationManagerMaster = nullptr;
   }
}

std::shared_ptr<ReplicationObject> ObjectReplicationCreate(uint32_t classId)
{
   std::shared_ptr<ReplicationObject> pObj = BIEngine::NetworkObjectCreationRegistry::Get().Create(classId);
   pObj->Init(true);
   g_pObjectReplicationManagerMaster->AddReplicationObject(pObj);

   return pObj;
}

void ObjectReplicationManagerMaster::AddClient(std::shared_ptr<Peer> pPeer)
{
   m_pPeers.push_back(pPeer);
   std::unique_ptr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.emplace_back(std::make_unique<ReplicationActionWriter>(m_pLinkingContext));
   OutputMemoryBitStream& replicationBuffer = m_replicationBuffersPerPeer.emplace_back(OutputMemoryBitStream());

   for (const auto& pObj : m_pReplicationObjects) {
      pReplicationManager->ReplicateCreate(replicationBuffer, pObj);
   }
}

void ObjectReplicationManagerMaster::SendPacket(NetworkManager* pNetworkManager)
{
   for (int i = 0; i < m_replicationBuffersPerPeer.size(); ++i) {
      if (m_replicationBuffersPerPeer[i].GetBitLength() > 0) {
         SendStatePacketToClient(i, pNetworkManager);
         m_replicationBuffersPerPeer[i] = OutputMemoryBitStream();
      }
   }
}

void ObjectReplicationManagerMaster::OnUpdate()
{
   for (auto& obj : m_pReplicationObjects) {
      obj->OnUpdate();

      if (obj->IsDirty()) {
         for (int i = 0; i < m_pReplicationManagersPerPeer.size(); ++i) {
            m_pReplicationManagersPerPeer[i]->ReplicateUpdate(m_replicationBuffersPerPeer[i], obj);
         }
      }
   }
}

void ObjectReplicationManagerMaster::AddReplicationObject(std::shared_ptr<ReplicationObject> pObj)
{
   m_pReplicationObjects.push_back(pObj);

   for (int i = 0; i < m_pReplicationManagersPerPeer.size(); ++i) {
      m_pReplicationManagersPerPeer[i]->ReplicateCreate(m_replicationBuffersPerPeer[i], pObj);
   }
}

void ObjectReplicationManagerMaster::SendStatePacketToClient(int peerIdx, NetworkManager* pNetworkManager)
{
   // build state packet
   BIEngine::OutputMemoryBitStream statePacket;

   // it's state!
   BIEngine::Serialize(statePacket, 'STAT');

   // WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

   statePacket.WriteBits(m_replicationBuffersPerPeer[peerIdx].GetBufferPtr().get(), m_replicationBuffersPerPeer[peerIdx].GetBitLength());
   pNetworkManager->SendPacket(statePacket, m_pPeers[peerIdx]->GetSocketAddress());
}

} // namespace BIEngine
