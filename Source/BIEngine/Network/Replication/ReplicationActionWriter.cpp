#include "ReplicationActionWriter.h"

#include "ReplicationHeader.h"

namespace BIEngine {

void ReplicationActionWriter::ReplicateCreate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Create, m_pLinkingContext->GetId(pGameObject, true), pGameObject->GetClassType());
   rh.Write(stream);
   pGameObject->Write(stream, true);
}

void ReplicationActionWriter::ReplicateUpdate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Update, m_pLinkingContext->GetId(pGameObject, false), pGameObject->GetClassType());
   rh.Write(stream);
   pGameObject->Write(stream, false);
}

void ReplicationActionWriter::ReplicateDestroy(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject)
{
   ReplicationHeader rh(ReplicationAction::Destroy, m_pLinkingContext->GetId(pGameObject, false));
   rh.Write(stream);
}

} // namespace BIEngine
