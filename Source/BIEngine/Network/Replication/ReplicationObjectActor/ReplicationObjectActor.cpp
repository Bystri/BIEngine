#include "ReplicationObjectActor.h"

#include "../../EngineCore/GameApp.h"
#include "../../ResourceCache/XmlLoader.h"

namespace BIEngine {

std::shared_ptr<Actor> ReplicationObjectActor::ConstructReplicatedObject(bool isMaster)
{
   std::shared_ptr<XmlExtraData> pActorData;
   if (isMaster) {
      pActorData = std::static_pointer_cast<XmlExtraData>(ResCache::Get()->GetHandle(m_masterActorFilePath)->GetExtra());
   } else {
      pActorData = std::static_pointer_cast<XmlExtraData>(ResCache::Get()->GetHandle(m_slaveActorFilePath)->GetExtra());
   }

   return BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement());
}

} // namespace BIEngine
