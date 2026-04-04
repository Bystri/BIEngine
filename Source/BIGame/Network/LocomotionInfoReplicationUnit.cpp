#include "LocomotionInfoReplicationUnit.h"

#include "../../BIEngine/Actors/Actor.h"

void LocomotionInfoReplicationUnit::Init(BIEngine::ReplicationObject* pRelicationObject, BIEngine::SharedPtr<BIEngine::Actor> pObject)
{
   ReplicationUnit::Init(pRelicationObject, pObject);
   m_pLocomotionInfoComponent = pObject->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
}

static bool liRepUnitIsVector2Diff(const glm::vec2& a, const glm::vec2& b)
{
   constexpr float EPS = 0.001f;
   return glm::abs(a.x - b.x) > EPS || glm::abs(a.y - b.y) > EPS;
}

static bool liRepUnitIsVector3Diff(const glm::vec3& a, const glm::vec3& b)
{
   constexpr float EPS = 0.001f;
   return glm::abs(a.x - b.x) > EPS || glm::abs(a.y - b.y) > EPS || glm::abs(a.z - b.z) > EPS;
}

bool LocomotionInfoReplicationUnit::IsStateChanged()
{
   bool ret = liRepUnitIsVector3Diff(m_pLocomotionInfoComponent->GetCurrentVel(), m_cachedCurVel) || liRepUnitIsVector2Diff(m_pLocomotionInfoComponent->GetCurrentDir(), m_cachedCurDir) || liRepUnitIsVector3Diff(m_pLocomotionInfoComponent->GetInputVel(), m_cachedDesiredVel) || liRepUnitIsVector2Diff(m_pLocomotionInfoComponent->GetInputDir(), m_cachedDesiredDir);
   m_cachedCurVel = m_pLocomotionInfoComponent->GetCurrentVel();
   m_cachedCurDir = m_pLocomotionInfoComponent->GetCurrentDir();
   m_cachedDesiredVel = m_pLocomotionInfoComponent->GetInputVel();
   m_cachedDesiredDir = m_pLocomotionInfoComponent->GetInputDir();

   return ret;
}

void LocomotionInfoReplicationUnit::Write(BIEngine::OutputMemoryBitStream& stream)
{
   stream.WriteBytes(&m_cachedCurVel, sizeof(glm::vec3));
   stream.WriteBytes(&m_cachedCurDir, sizeof(glm::vec2));
   stream.WriteBytes(&m_cachedDesiredVel, sizeof(glm::vec3));
   stream.WriteBytes(&m_cachedDesiredDir, sizeof(glm::vec2));
}

void LocomotionInfoReplicationUnit::Read(BIEngine::InputMemoryBitStream& stream)
{
   glm::vec3 curVel, desriedVel;
   glm::vec2 curDir, desiredDir;
   stream.ReadBytes(&curVel, sizeof(glm::vec3));
   stream.ReadBytes(&curDir, sizeof(glm::vec2));
   stream.ReadBytes(&desriedVel, sizeof(glm::vec3));
   stream.ReadBytes(&desiredDir, sizeof(glm::vec2));

   m_pLocomotionInfoComponent->SetCurrentVel(curVel);
   m_pLocomotionInfoComponent->SetCurrentDir(curDir);
   m_pLocomotionInfoComponent->SetInputVel(desriedVel);
   m_pLocomotionInfoComponent->SetInputDir(desiredDir);
}