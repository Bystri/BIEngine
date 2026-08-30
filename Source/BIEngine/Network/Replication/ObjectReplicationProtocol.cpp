#include "ObjectReplicationProtocol.h"

#include <imgui.h>

#include "../../Utilities/Logger.h"
#include "../../Utilities/DebugDraw.h"
#include "../../Actors/TransformComponent.h"

namespace BIEngine {

const NetworkProtocolType ObjectReplicationProtocolLeader::sk_ProtocolType(0x23d7aeaa);
const NetworkProtocolType ObjectReplicationProtocolFollower::sk_ProtocolType(0x23d7aeaa);

/***ObjectReplicationProtocolWriter***/

static ObjectReplicationProtocolLeader* g_pObjectReplicationProtocol;

ObjectReplicationProtocolLeader* ObjectReplicationProtocolLeader::Get()
{
   Assert(g_pObjectReplicationProtocol != nullptr, "You are trying to get ObjectReplicationProtocolWriter before it creation");

   return g_pObjectReplicationProtocol;
}

ObjectReplicationProtocolLeader::ObjectReplicationProtocolLeader()
   : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>())
{
   if (g_pObjectReplicationProtocol) {
      Logger::WriteErrorLog("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.\n");
   }

   g_pObjectReplicationProtocol = this;
}

ObjectReplicationProtocolLeader::~ObjectReplicationProtocolLeader()
{
   if (g_pObjectReplicationProtocol == this) {
      g_pObjectReplicationProtocol = nullptr;
   }
}

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId)
{
   SharedPtr<ReplicationObject> pObj = BIEngine::NetworkObjectCreationRegistry::Get().Create(classId);
   ObjectReplicationProtocolLeader::Get()->AddReplicationObject(pObj);
   pObj->Init(g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId());

   return pObj;
}

void ObjectReplicationDestroy(SharedPtr<ReplicationObject> pGameObject)
{
   pGameObject->Term();
   ObjectReplicationProtocolLeader::Get()->DestroyReplicationObject(pGameObject);
}

void ObjectReplicationProtocolLeader::AddObjectReplicationPOI(PeerId peerId, SharedPtr<Actor> pActorPOI, float softRadius, float hardRadius)
{
   auto itr = m_relevancyInfo.Find(peerId);

   ReplicationRelevancyInfo& info = itr->second;
   info.pActorPOI = pActorPOI;
   info.softRadius = softRadius;
   info.hardRadius = hardRadius;
}

void ObjectReplicationProtocolLeader::RemoveObjectReplicationPOI(PeerId peerId)
{
   auto itr = m_relevancyInfo.Find(peerId);
   Assert(itr != m_relevancyInfo.End(), "You are trying to delete unregistered POI");
   m_relevancyInfo.Erase(peerId);
}

#ifndef _RETAIL
void ObjectReplicationProtocolLeader::DrawDbgDiagnostics() const
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

void ObjectReplicationProtocolLeader::AddReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   const uint32_t objId = m_pLinkingContext->GetId(pObj, true);
   pObj->SetNetworkId(objId);
   m_pReplicationObjects.PushBack(pObj);
}

void ObjectReplicationProtocolLeader::DestroyReplicationObject(SharedPtr<ReplicationObject> pObj)
{
   const uint32_t objId = m_pLinkingContext->GetId(pObj, false);

   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      const bool isObjReplicatedToPoi = m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Find(objId) != m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.End();

      if (isObjReplicatedToPoi) {
         m_pReplicationManagersPerPeer[i]->ReplicateDestroy(pObj);
         m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Erase(objId);
      }
   }

   for (int i = 0; i < m_pReplicationObjects.Size(); ++i) {
      if (m_pReplicationObjects[i].Get() == pObj.Get()) {
         m_pReplicationObjects.Erase(m_pReplicationObjects.Begin() + i);
         break;
      }
   }

   m_pLinkingContext->RemoveObj(pObj);
}

void ObjectReplicationProtocolLeader::SendStateMsgToClient(PeerId peerId, NetworkMessagesManager* pNetworkMessagesManager)
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

void ObjectReplicationProtocolLeader::RegisterPeer(PeerId peerId)
{
   m_pPeers.PushBack(peerId);
   UniquePtr<ReplicationActionWriter>& pReplicationManager = m_pReplicationManagersPerPeer.EmplaceBack(MakeUnique<ReplicationActionWriter>(m_pLinkingContext));
   m_relevancyInfo.Emplace(peerId, ReplicationRelevancyInfo());
}

void ObjectReplicationProtocolLeader::UnregisterPeer(PeerId peerId)
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

void ObjectReplicationProtocolLeader::UpdateReplicatedObjectsState()
{
    for (auto& obj : m_pReplicationObjects) {
        if (obj->GetMasterPeerId() != g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId()) {
            continue;
        }

        obj->OnUpdate();

        for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
            auto itr = m_relevancyInfo.Find(m_pPeers[i]);
            ReplicationRelevancyInfo& relInfo = itr->second;

            const uint32_t objId = m_pLinkingContext->GetId(obj, false);

            const bool isObjReplicatedToPoi = m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Find(objId) != m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.End();

            if (relInfo.pActorPOI == nullptr || !obj->IsUseRelevancy()) {
                if (!isObjReplicatedToPoi) {
                    Logger::WriteMsgLog("Send replication action create of NON RELEVANCE object [NetworkId:%u] to peer [PeerId:%u]", objId, m_pPeers[i]);

                    m_pReplicationManagersPerPeer[i]->ReplicateCreate(obj);
                    m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Insert(objId);
                }
                else {
                    if (obj->IsDirty()) {
                        m_pReplicationManagersPerPeer[i]->ReplicateUpdate(obj);
                    }
                }

                continue;
            }

            const glm::vec3& poiPos = m_relevancyInfo[m_pPeers[i]].pActorPOI->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock()->GetPosition();
            const float dist = glm::length(poiPos - obj->GetPosition()); // TODO: lengthSqr

            if (!isObjReplicatedToPoi) {
                if (dist < m_relevancyInfo[m_pPeers[i]].softRadius) {
                    Logger::WriteMsgLog("Send replication action create of object [NetworkId:%u] to peer [PeerId:%u]", objId, m_pPeers[i]);
                    m_pReplicationManagersPerPeer[i]->ReplicateCreate(obj);
                    m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Insert(objId);
                }
            }
            else {
                if (dist < m_relevancyInfo[m_pPeers[i]].hardRadius) {
                    if (obj->IsDirty()) {
                        m_pReplicationManagersPerPeer[i]->ReplicateUpdate(obj);
                    }
                }
                else {
                    m_pReplicationManagersPerPeer[i]->ReplicateDestroy(obj);
                    m_relevancyInfo[m_pPeers[i]].replicatedObjsSet.Erase(objId);
                }
            }
        }
    }
}

void ObjectReplicationProtocolLeader::OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager)
{
   UpdateReplicatedObjectsState();

   for (int i = 0; i < m_pReplicationManagersPerPeer.Size(); ++i) {
      if (m_pReplicationManagersPerPeer[i]->GetNumOfCachedHeaders() > 0) {
         SendStateMsgToClient(m_pPeers[i], pNetworkMessagesManager);
      }
   }
}

/***ObjectReplicationProtocolReader***/

static ObjectReplicationProtocolFollower* g_pObjectReplicationReaderProtocol;

ObjectReplicationProtocolFollower* ObjectReplicationProtocolFollower::Get()
{
   Assert(g_pObjectReplicationReaderProtocol != nullptr, "You are trying to get ObjectReplicationProtocolReader before it creation");

   return g_pObjectReplicationReaderProtocol;
}

ObjectReplicationProtocolFollower::ObjectReplicationProtocolFollower()
   : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>())
{
   if (g_pObjectReplicationReaderProtocol) {
      Logger::WriteErrorLog("Attempting to create two global ObjectReplicationProtocolReaders! The old one will be destroyed and overwritten with this one.\n");
   }

   g_pObjectReplicationReaderProtocol = this;
}

ObjectReplicationProtocolFollower::~ObjectReplicationProtocolFollower()
{
   if (g_pObjectReplicationReaderProtocol == this) {
      g_pObjectReplicationReaderProtocol = nullptr;
   }
}

void ObjectReplicationProtocolFollower::ReceiveMessage(InputMemoryBitStream& stream)
{
   uint32_t numOfHeaders;
   Deserialize(stream, numOfHeaders);

   for (int i = 0; i < numOfHeaders; ++i) {
      if (!ProcessReplicationHeader(stream)) {
         Logger::WriteErrorLog("Malformed object replication message. Remaining replication headers skipped.");
         return;
      }
   }
}

bool ObjectReplicationProtocolFollower::ProcessReplicationHeader(InputMemoryBitStream& stream)
{
   ReplicationHeader rh;
   if (!rh.Read(stream)) {
      return false;
   }

   if (rh.GetMasterPeerId() == g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId()) {
      Logger::WriteErrorLog("ObjectReplicationProtocolFollower got update for owned entity with newtworkId %d. Update skipped.", rh.GetNetworkId());
      return true;
   }

   switch (rh.GetReplicationAction()) {
      case ReplicationAction::Create:
         {
            Logger::WriteMsgLog("Create replicated object [ClassId: %u] - [NetworkID: %u]", rh.GetClassId(), rh.GetNetworkId());

            SharedPtr<ReplicationObject> go = NetworkObjectCreationRegistry::Get().Create(rh.GetClassId());
            if (go == nullptr) {
               Logger::WriteErrorLog("Attempt to create an unknown replicated object [ClassId:%u]", rh.GetClassId());
               break;
            }

            m_pLinkingContext->AddObj(go, rh.GetNetworkId());
            go->SetNetworkId(rh.GetNetworkId());
            go->Init(rh.GetMasterPeerId());

            InputMemoryBitStream payloadStream = rh.GetPayloadStream();
            go->Read(payloadStream);
            if (payloadStream.HasReadError()) {
               Logger::WriteErrorLog("Malformed create payload for replicated object [NetworkID:%u]", rh.GetNetworkId());
            }

            break;
         }
      case ReplicationAction::Update:
         {
            SharedPtr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());

            if (go) {
               InputMemoryBitStream payloadStream = rh.GetPayloadStream();
               go->Read(payloadStream);
               if (payloadStream.HasReadError()) {
                  Logger::WriteErrorLog("Malformed update payload for replicated object [NetworkID:%u]", rh.GetNetworkId());
               }
            } else {
               Logger::WriteErrorLog("Got update for an unknown replicated object [NetworkID:%u]. Payload skipped.", rh.GetNetworkId());
            }
            break;
         }
      case ReplicationAction::Destroy:
         {
            Logger::WriteMsgLog("Delete replicated object [NetworkID: %u]", rh.GetNetworkId());

            SharedPtr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());
            go->Term();
            m_pLinkingContext->RemoveObj(go);

            break;
         }
      default:
         // not handled by us
         break;
   }

   return true;
}

} // namespace BIEngine
