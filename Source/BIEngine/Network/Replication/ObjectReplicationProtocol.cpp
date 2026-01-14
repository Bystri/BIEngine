#include "ObjectReplicationProtocol.h"

#include "../../Utilities/Logger.h"

namespace BIEngine {

const NetworkProtocolType ObjectReplicationProtocolWriter::sk_ProtocolType(0x23d7aeaa);
const NetworkProtocolType ObjectReplicationProtocolReader::sk_ProtocolType(0x23d7aeaa);

/***ObjectReplicationProtocolWriter***/

static ObjectReplicationProtocolWriter* g_pObjectReplicationProtocol;

ObjectReplicationProtocolWriter* ObjectReplicationProtocolWriter::Get()
{
   Assert(g_pObjectReplicationProtocol != nullptr, "You are trying to get ObjectReplicationProtocolWriter before it creation");

   return g_pObjectReplicationProtocol;
}

ObjectReplicationProtocolWriter::ObjectReplicationProtocolWriter()
   : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>())
{
   if (g_pObjectReplicationProtocol) {
      Logger::WriteErrorLog("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.\n");
   }

   g_pObjectReplicationProtocol = this;
}

ObjectReplicationProtocolWriter::~ObjectReplicationProtocolWriter()
{
   if (g_pObjectReplicationProtocol == this) {
      g_pObjectReplicationProtocol = nullptr;
   }
}

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId)
{
   SharedPtr<ReplicationObject> pObj = BIEngine::NetworkObjectCreationRegistry::Get().Create(classId);
   pObj->Init(true);
   ObjectReplicationProtocolWriter::Get()->AddReplicationObject(pObj);

   return pObj;
}

void ObjectReplicationProtocolWriter::OnUpdate()
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

void ObjectReplicationProtocolWriter::AddReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   m_pReplicationObjects.PushBack(pObj);

   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      m_pReplicationManagersPerPeer[i]->ReplicateCreate(m_replicationBuffersPerPeer[i], pObj);
   }
}

void ObjectReplicationProtocolWriter::SendStatePacketToClient(int peerIdx, NetworkManager* pNetworkManager)
{
   // WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);
   pNetworkManager->SendNetworkMessage(*m_pPeers[peerIdx], GetType(), m_replicationBuffersPerPeer[peerIdx]);
}

void ObjectReplicationProtocolWriter::RegisterPeer(PeerPtr pPeer)
{
   m_pPeers.PushBack(pPeer);
   UniquePtr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.EmplaceBack(MakeUnique<ReplicationActionWriter>(m_pLinkingContext));
   OutputMemoryBitStream& replicationBuffer = m_replicationBuffersPerPeer.EmplaceBack(OutputMemoryBitStream());

   for (const auto& pObj : m_pReplicationObjects) {
      pReplicationManager->ReplicateCreate(replicationBuffer, pObj);
   }
}

void ObjectReplicationProtocolWriter::UnregisterPeer(PeerPtr pPeer)
{
   for (int i = 0; i < m_pPeers.Size(); ++i) {
      if (m_pPeers[i] == pPeer) {
         m_pPeers.Erase(m_pPeers.Begin() + i);
         m_pReplicationManagersPerPeer.Erase(m_pReplicationManagersPerPeer.Begin() + i);
         m_replicationBuffersPerPeer.Erase(m_replicationBuffersPerPeer.Begin() + i);

         return;
      }
   }
}

void ObjectReplicationProtocolWriter::OnBeforePacketsSend(NetworkManager* pNetworkManager)
{
   for (int i = 0; i < m_replicationBuffersPerPeer.Size(); ++i) {
      if (m_replicationBuffersPerPeer[i].GetBitLength() > 0) {
         SendStatePacketToClient(i, pNetworkManager);
         m_replicationBuffersPerPeer[i] = OutputMemoryBitStream();
      }
   }
}

/***ObjectReplicationProtocolReader***/

void ObjectReplicationProtocolReader::ReceiveMessage(InputMemoryBitStream& stream)
{
   while (stream.GetRemainingBitCount() >= 32) {
      m_pReplicationActionReader->ProcessReplicationAction(stream);
   }
}

} // namespace BIEngine
