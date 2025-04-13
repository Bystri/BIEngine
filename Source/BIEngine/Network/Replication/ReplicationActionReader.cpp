#include "ReplicationActionReader.h"

#include "ReplicationHeader.h"
#include "../../Utilities/Logger.h"

namespace BIEngine {

void ReplicationActionReader::ProcessReplicationAction(InputMemoryBitStream& stream)
{
   ReplicationHeader rh;
   rh.Read(stream);
   switch (rh.GetReplicationAction()) {
      case ReplicationAction::Create:
         {
            std::shared_ptr<ReplicationObject> go = NetworkObjectCreationRegistry::Get().Create(rh.GetClassId());
            m_pLinkingContext->AddObj(go, rh.GetNetworkId());
            go->Init(false);
            go->Read(stream);
            Logger::WriteMsgLog("ReplicationAction::Create for id %d", rh.GetNetworkId());
            break;
         }
      case ReplicationAction::Update:
         {
            std::shared_ptr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());
            // we might have not received the create yet,
            // so serialize into a dummy to advance read head
            if (go) {
               go->Read(stream);
            } else {
               uint32_t classId = rh.GetClassId();
               go = NetworkObjectCreationRegistry::Get().Create(classId);
               go->Read(stream);
            }
            Logger::WriteMsgLog("ReplicationAction::Update for id %d", rh.GetNetworkId());
            break;
         }
      case ReplicationAction::Destroy:
         {
            std::shared_ptr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());
            m_pLinkingContext->RemoveObj(go);
            Logger::WriteMsgLog("ReplicationAction::Destroy for id %d", rh.GetNetworkId());
            break;
         }
      default:
         // not handled by us
         break;
   }
}

} // namespace BIEngine
