#include "CombatReplicationUnit.h"

#include "../../BIEngine/Actors/Actor.h"

void CombatReplicationUnit::Init(BIEngine::ReplicationObject* pRelicationObject, BIEngine::SharedPtr<BIEngine::Actor> pObject)
{
   ReplicationUnit::Init(pRelicationObject, pObject);
   m_pCombatStateComponent = pObject->GetComponent<CombatStateComponent>(CombatStateComponent::g_CompId).Lock();
}

bool CombatReplicationUnit::IsStateChanged()
{
   bool ret = m_cachedIsAttackInProgress != m_pCombatStateComponent->IsAttackInProgress();
   m_cachedIsAttackInProgress = m_pCombatStateComponent->IsAttackInProgress();

   return ret;
}

void CombatReplicationUnit::Write(BIEngine::OutputMemoryBitStream& stream)
{
   BIEngine::Serialize(stream, m_cachedIsAttackInProgress);
}

void CombatReplicationUnit::Read(BIEngine::InputMemoryBitStream& stream)
{
   bool isAttackInProgress;
   BIEngine::Deserialize(stream, isAttackInProgress);

   m_pCombatStateComponent->SetIsAttackInProgress(isAttackInProgress);
}
