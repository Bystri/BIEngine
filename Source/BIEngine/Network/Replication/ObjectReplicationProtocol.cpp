#include "ObjectReplicationProtocol.h"

#include "../../Utilities/Logger.h"

namespace BIEngine {

#pragma optimize("", off)

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
   pObj->Init(g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId());
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

void ObjectReplicationProtocolWriter::SendStateMsgToClient(PeerId peerId, NetworkMessagesManager* pNetworkMessagesManager)
{
   OutputMemoryBitStream msg;

   for (int i = 0; i < m_pPeers.Size(); ++i) {
      if (m_pPeers[i] != peerId) {
         continue;
      }

      m_pReplicationManagersPerPeer[i]->Write(msg);
      pNetworkMessagesManager->SendNetworkMessage(peerId, GetType(), msg);

      return;
   }

   Logger::WriteErrorLog("Trying to send ObjectReplication info to unknown peerid [%u]", peerId);
}

void ObjectReplicationProtocolWriter::RegisterPeer(PeerId peerId)
{
   m_pPeers.PushBack(peerId);
   UniquePtr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.EmplaceBack(MakeUnique<ReplicationActionWriter>(m_pLinkingContext));

   for (const auto& pObj : m_pReplicationObjects) {
      pReplicationManager->ReplicateCreate(pObj);
   }
}

void ObjectReplicationProtocolWriter::UnregisterPeer(PeerId peerId)
{
   for (int i = 0; i < m_pPeers.Size(); ++i) {
      if (m_pPeers[i] == peerId) {
         m_pPeers.Erase(m_pPeers.Begin() + i);
         m_pReplicationManagersPerPeer.Erase(m_pReplicationManagersPerPeer.Begin() + i);

         return;
      }
   }
}

void ObjectReplicationProtocolWriter::OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager)
{
   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      if (m_pReplicationManagersPerPeer[i]->GetNumOfCachedHeaders() > 0) {
         SendStateMsgToClient(m_pPeers[i], pNetworkMessagesManager);
      }
   }
}

/***ObjectReplicationProtocolReader***/

void ObjectReplicationProtocolReader::ReceiveMessage(InputMemoryBitStream& stream)
{
   m_pReplicationActionReader->ProcessReplicationActions(stream);
}

} // namespace BIEngine
