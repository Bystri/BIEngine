#include "ReplicationActionReader.h"

#include "ReplicationHeader.h"
#include "../../Utilities/Logger.h"

namespace BIEngine {

void ReplicationActionReader::ProcessReplicationActions(InputMemoryBitStream& stream)
{
   uint32_t numOfHeaders;
   Deserialize(stream, numOfHeaders);

   for (int i = 0; i < numOfHeaders; ++i) {
      ProcessReplicationHeader(stream);
   }
}

void ReplicationActionReader::ProcessReplicationHeader(InputMemoryBitStream& stream)
{
   ReplicationHeader rh;
   rh.Read(stream);

   switch (rh.GetReplicationAction()) {
      case ReplicationAction::Create:
         {
            SharedPtr<ReplicationObject> go = NetworkObjectCreationRegistry::Get().Create(rh.GetClassId());
            m_pLinkingContext->AddObj(go, rh.GetNetworkId());
            go->Init(rh.GetMasterPeerId());
            go->Read(stream);
            break;
         }
      case ReplicationAction::Update:
         {
            SharedPtr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());
            // we might have not received the create yet,
            // so serialize into a dummy to advance read head
            if (go) {
               go->Read(stream);
            } else {
               uint32_t classId = rh.GetClassId();
               go = NetworkObjectCreationRegistry::Get().Create(classId);
               go->Read(stream);
            }
            break;
         }
      case ReplicationAction::Destroy:
         {
            SharedPtr<ReplicationObject> go = m_pLinkingContext->GetObj(rh.GetNetworkId());
            go->Term();
            m_pLinkingContext->RemoveObj(go);
            break;
         }
      default:
         // not handled by us
         break;
   }
}

} // namespace BIEngine
