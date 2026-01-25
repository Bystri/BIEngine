#include "ReplicationActionWriter.h"

namespace BIEngine {

void ReplicationActionWriter::ReplicateCreate(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(ReplicationAction::Create, m_pLinkingContext->GetId(pGameObject, true), pGameObject);
}

void ReplicationActionWriter::ReplicateUpdate(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(ReplicationAction::Update, m_pLinkingContext->GetId(pGameObject, false), pGameObject);
}

void ReplicationActionWriter::ReplicateDestroy(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(ReplicationAction::Destroy, m_pLinkingContext->GetId(pGameObject, false));
}

} // namespace BIEngine
