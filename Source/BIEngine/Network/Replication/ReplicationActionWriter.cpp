#include "ReplicationActionWriter.h"

namespace BIEngine {

void ReplicationActionWriter::ReplicateCreate(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(
       ReplicationAction::Create, 
       g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId(),
       m_pLinkingContext->GetId(pGameObject, true), pGameObject);
}

void ReplicationActionWriter::ReplicateUpdate(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(
       ReplicationAction::Update, 
       g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId(), 
       m_pLinkingContext->GetId(pGameObject, false), pGameObject);
}

void ReplicationActionWriter::ReplicateDestroy(SharedPtr<ReplicationObject> pGameObject)
{
   m_replicationHeadersBuffer.Emplace(
       ReplicationAction::Destroy, 
       g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId(),
       m_pLinkingContext->GetId(pGameObject, false));
}

} // namespace BIEngine
