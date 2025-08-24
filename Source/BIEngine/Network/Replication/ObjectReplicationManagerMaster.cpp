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
   : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>())
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

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId)
{
   SharedPtr<ReplicationObject> pObj = BIEngine::NetworkObjectCreationRegistry::Get().Create(classId);
   pObj->Init(true);
   g_pObjectReplicationManagerMaster->AddReplicationObject(pObj);

   return pObj;
}

void ObjectReplicationManagerMaster::AddClient(SharedPtr<Peer> pPeer)
{
   m_pPeers.PushBack(pPeer);
   UniquePtr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.EmplaceBack(MakeUnique<ReplicationActionWriter>(m_pLinkingContext));
   OutputMemoryBitStream& replicationBuffer = m_replicationBuffersPerPeer.EmplaceBack(OutputMemoryBitStream());

   for (const auto& pObj : m_pReplicationObjects) {
      pReplicationManager->ReplicateCreate(replicationBuffer, pObj);
   }
}

void ObjectReplicationManagerMaster::SendPacket(NetworkManager* pNetworkManager)
{
   for (int i = 0; i < m_replicationBuffersPerPeer.Size(); ++i) {
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
         for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
            m_pReplicationManagersPerPeer[i]->ReplicateUpdate(m_replicationBuffersPerPeer[i], obj);
         }
      }
   }
}

void ObjectReplicationManagerMaster::AddReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   m_pReplicationObjects.PushBack(pObj);

   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
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

   statePacket.WriteBits(m_replicationBuffersPerPeer[peerIdx].GetBufferPtr().Get(), m_replicationBuffersPerPeer[peerIdx].GetBitLength());
   pNetworkManager->SendPacket(statePacket, m_pPeers[peerIdx]->GetSocketAddress());
}

} // namespace BIEngine
