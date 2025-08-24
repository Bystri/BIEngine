#include "ReplicationObjectActor.h"

#include "../../EngineCore/GameApp.h"
#include "../../ResourceCache/XmlLoader.h"

namespace BIEngine {

SharedPtr<Actor> ReplicationObjectActor::ConstructReplicatedObject(bool isMaster)
{
   SharedPtr<XmlExtraData> pActorData;
   if (isMaster) {
      pActorData = StaticPointerCast<XmlExtraData>(ResCache::Get()->GetHandle(m_masterActorFilePath)->GetExtra());
   } else {
      pActorData = StaticPointerCast<XmlExtraData>(ResCache::Get()->GetHandle(m_slaveActorFilePath)->GetExtra());
   }

   return BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement());
}

} // namespace BIEngine
