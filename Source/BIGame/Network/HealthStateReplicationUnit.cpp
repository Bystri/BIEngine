#include "HealthStateReplicationUnit.h"

#include "../../BIEngine/Actors/Actor.h"

void HealthStateReplicationUnit::Init(BIEngine::SharedPtr<BIEngine::Actor> pObject)
{
   m_pHealthStateComponent = pObject->GetComponent<HealthStateComponent>(HealthStateComponent::g_CompId).Lock();
}

bool HealthStateReplicationUnit::IsStateChanged()
{
   bool ret = m_cachedIsDead != m_pHealthStateComponent->IsDead();
   m_cachedIsDead = m_pHealthStateComponent->IsDead();

   ret |= abs(m_cachedHealth - m_pHealthStateComponent->GetHealth()) > std::numeric_limits<float>::epsilon();
   m_cachedHealth = m_pHealthStateComponent->GetHealth();

   return ret;
}

void HealthStateReplicationUnit::Write(BIEngine::OutputMemoryBitStream& stream)
{
   BIEngine::Serialize(stream, m_cachedIsDead, 1);
   BIEngine::Serialize(stream, m_cachedHealth);
}

void HealthStateReplicationUnit::Read(BIEngine::InputMemoryBitStream& stream)
{
   bool isDead;
   BIEngine::Deserialize(stream, isDead, 1);
   m_pHealthStateComponent->SetIsDead(isDead);

   float health;
   BIEngine::Deserialize(stream, health);
   m_pHealthStateComponent->SetHealth(health);
}
