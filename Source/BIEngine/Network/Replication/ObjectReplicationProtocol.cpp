#include "ObjectReplicationProtocol.h"

#include <imgui.h>

#include "../../Utilities/Logger.h"
#include "../../Utilities/DebugDraw.h"
#include "../../Actors/TransformComponent.h"

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

void ObjectReplicationDestroy(SharedPtr<ReplicationObject> pGameObject)
{
   pGameObject->Term();
   ObjectReplicationProtocolWriter::Get()->DestroyReplicationObject(pGameObject);
}

void ObjectReplicationProtocolWriter::AddObjectReplicationPOI(PeerId peerId, SharedPtr<Actor> pActorPOI, float softRadius, float hardRadius)
{
   auto itr = m_relevancyInfo.Find(peerId);

   ReplicationRelevancyInfo& info = itr->second;
   info.pActorPOI = pActorPOI;
   info.softRadius = softRadius;
   info.hardRadius = hardRadius;
}

void ObjectReplicationProtocolWriter::RemoveObjectReplicationPOI(PeerId peerId)
{
   auto itr = m_relevancyInfo.Find(peerId);
   Assert(itr != m_relevancyInfo.End(), "You are trying to delete unregistered POI");
   m_relevancyInfo.Erase(peerId);
}

void ObjectReplicationProtocolWriter::OnUpdate()
{
   for (auto& obj : m_pReplicationObjects) {
      obj->OnUpdate();

      for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
         auto itr = m_relevancyInfo.Find(m_pPeers[i]);
         ReplicationRelevancyInfo& relInfo = itr->second;

         const uint32_t objId = m_pLinkingContext->GetId(obj, false);

         const bool isObjReplicatedToPoi = m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Find(objId) != m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.End();

         if (relInfo.pActorPOI == nullptr || !obj->IsUseRelevancy()) {
             if (!isObjReplicatedToPoi) {
               m_pReplicationManagersPerPeer[i]->ReplicateCreate(obj);
               m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Insert(objId);
             } else {
                if (obj->IsDirty()) {
                   m_pReplicationManagersPerPeer[i]->ReplicateUpdate(obj);
                }
             }

             continue;
         }

         const glm::vec3& poiPos = m_relevancyInfo[m_pPeers[i]].pActorPOI->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock()->GetPosition();
         const float dist = glm::length(poiPos - obj->GetPosition()); // TODO: lengthSqr

         if (dist < m_relevancyInfo[m_pPeers[i]].hardRadius) {
            if (!isObjReplicatedToPoi) {
               m_pReplicationManagersPerPeer[i]->ReplicateCreate(obj);
               m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Insert(objId);
            } else {
               if (obj->IsDirty()) {
                  m_pReplicationManagersPerPeer[i]->ReplicateUpdate(obj);
               }
            }
         } else if (isObjReplicatedToPoi) {
            m_pReplicationManagersPerPeer[i]->ReplicateDestroy(obj);
            m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Erase(objId);
         }
      }
   }
}

#ifndef _RETAIL
void ObjectReplicationProtocolWriter::DrawDbgDiagnostics() const
{
   ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_Always);

   if (!ImGui::Begin("Replication info")) {
      ImGui::End();
      return;
   }

   for (const auto& info : m_relevancyInfo) {
      const glm::vec3& pos = info.second.pActorPOI->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock()->GetPosition();
      BIEngine::DebugDraw::Sphere(
         pos,
         info.second.softRadius,
         BIEngine::COLOR_GREEN, 0.0f, false);

      BIEngine::DebugDraw::Sphere(
         pos,
         info.second.hardRadius,
         BIEngine::COLOR_RED, 0.0f, false);
   }

   ImGui::End();
}
#endif

void ObjectReplicationProtocolWriter::AddReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   const uint32_t objId = m_pLinkingContext->GetId(pObj, true);
   m_pReplicationObjects.PushBack(pObj);
}

void ObjectReplicationProtocolWriter::DestroyReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      const uint32_t objId = m_pLinkingContext->GetId(pObj, false);

      const bool isObjReplicatedToPoi = m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Find(objId) != m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.End();

      if (isObjReplicatedToPoi) {
         m_pReplicationManagersPerPeer[i]->ReplicateDestroy(pObj);
         m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Erase(objId);
      }
   }

   for (int i = 0; i < m_pReplicationObjects.Size(); ++i) {
      if (m_pReplicationObjects[i].Get() == pObj.Get()) {
         m_pReplicationObjects.Erase(m_pReplicationObjects.Begin() + i);
         return;
      }
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
   m_relevancyInfo.Emplace(peerId, ReplicationRelevancyInfo());
}

void ObjectReplicationProtocolWriter::UnregisterPeer(PeerId peerId)
{
   for (int i = 0; i < m_pPeers.Size(); ++i) {
      if (m_pPeers[i] == peerId) {
         m_pPeers.Erase(m_pPeers.Begin() + i);
         m_pReplicationManagersPerPeer.Erase(m_pReplicationManagersPerPeer.Begin() + i);
         m_relevancyInfo.Erase(peerId);
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
