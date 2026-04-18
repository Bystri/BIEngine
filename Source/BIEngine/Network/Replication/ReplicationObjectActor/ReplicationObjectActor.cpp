#include "ReplicationObjectActor.h"

#include "../../EngineCore/GameApp.h"
#include "../../ResourceCache/XmlLoader.h"
#include "../../Actors/NetworkReplicatedComponent.h"

namespace BIEngine {

SharedPtr<Actor> ReplicationObjectActor::ConstructReplicatedObject(bool isMaster)
{
   SharedPtr<XmlExtraData> pActorData;
   if (isMaster) {
      pActorData = StaticPointerCast<XmlExtraData>(ResCache::Get()->GetHandle(m_masterActorFilePath)->GetExtra());
   } else {
      pActorData = StaticPointerCast<XmlExtraData>(ResCache::Get()->GetHandle(m_slaveActorFilePath)->GetExtra());
   }

   SharedPtr<Actor> pCreatedActor = BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement());

   if (isMaster) {
      SharedPtr<ReplicationObject> pSharedThis = SharedFromThis();
      pCreatedActor->GetComponent<NetworkReplicatedComponent>(NetworkReplicatedComponent::g_CompId).Lock()->SetReplicationObject(pSharedThis);
   }

   return pCreatedActor;
}

void ReplicationObjectActor::DestructReplicationObject(bool isMaster)
{
   if (!isMaster) {
      BIEngine::g_pApp->m_pGameLogic->DestroyActor(GetReplicatedObject()->GetId());
   }
}

} // namespace BIEngine
