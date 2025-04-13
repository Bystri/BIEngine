#include "AttachedActorReplicationUnit.h"

#include "../../BIEngine/EngineCore/GameApp.h"

void AttachedActorReplicationUnit::Init(std::shared_ptr<Player> pObject)
{
   m_pPlayer = pObject;
}

bool AttachedActorReplicationUnit::IsStateChanged()
{
   BIEngine::ActorId curActorId = BIEngine::Actor::INVALID_ACTOR_ID;
   if (m_pPlayer->GetPlayableActor() != nullptr) {
      curActorId = m_pPlayer->GetPlayableActor()->GetId();
   }

   const bool ret = curActorId != m_cachedActorId;
   m_cachedActorId = curActorId;

   return ret;
}

void AttachedActorReplicationUnit::Write(BIEngine::OutputMemoryBitStream& stream)
{
   BIEngine::Serialize(stream, m_cachedActorId);
}

void AttachedActorReplicationUnit::Read(BIEngine::InputMemoryBitStream& stream)
{
   BIEngine::ActorId actorId;
   BIEngine::Deserialize(stream, actorId);
   m_pPlayer->SetPlayableActor(BIEngine::g_pApp->m_pGameLogic->GetActor(actorId));
}
