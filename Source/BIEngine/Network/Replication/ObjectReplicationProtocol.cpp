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
            m_pReplicationManagersPerPeer[i]->ReplicateUpdate(obj);
         }
      }
   }
}

void ObjectReplicationProtocolWriter::AddReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   m_pReplicationObjects.PushBack(pObj);

   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      m_pReplicationManagersPerPeer[i]->ReplicateCreate(pObj);
   }
}

void ObjectReplicationProtocolWriter::SendStateMsgToClient(int peerIdx, NetworkManager* pNetworkManager)
{
   OutputMemoryBitStream msg;
   m_pReplicationManagersPerPeer[peerIdx]->Write(msg);
   pNetworkManager->SendNetworkMessage(*m_pPeers[peerIdx], GetType(), msg);
}

void ObjectReplicationProtocolWriter::RegisterPeer(PeerPtr pPeer)
{
   m_pPeers.PushBack(pPeer);
   UniquePtr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.EmplaceBack(MakeUnique<ReplicationActionWriter>(m_pLinkingContext));

   for (const auto& pObj : m_pReplicationObjects) {
      pReplicationManager->ReplicateCreate(pObj);
   }
}

void ObjectReplicationProtocolWriter::UnregisterPeer(PeerPtr pPeer)
{
   for (int i = 0; i < m_pPeers.Size(); ++i) {
      if (m_pPeers[i] == pPeer) {
         m_pPeers.Erase(m_pPeers.Begin() + i);
         m_pReplicationManagersPerPeer.Erase(m_pReplicationManagersPerPeer.Begin() + i);

         return;
      }
   }
}

void ObjectReplicationProtocolWriter::OnBeforePacketsSend(NetworkManager* pNetworkManager)
{
   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      if (m_pReplicationManagersPerPeer[i]->GetNumOfCachedHeaders() > 0) {
         SendStateMsgToClient(i, pNetworkManager);
      }
   }
}

/***ObjectReplicationProtocolReader***/

void ObjectReplicationProtocolReader::ReceiveMessage(InputMemoryBitStream& stream)
{
   m_pReplicationActionReader->ProcessReplicationActions(stream);
}

} // namespace BIEngine
