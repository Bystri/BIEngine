#include "TransformReplicationUnit.h"

#include "../../Actors/Actor.h"

namespace BIEngine {

void TransformReplicationUnit::Init(std::shared_ptr<Actor> pActor)
{
   m_pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
}

bool TransformReplicationUnit::IsStateChanged()
{
   const glm::vec3& pos = m_pTransformComponent->GetPosition();
   const glm::vec3& rot = m_pTransformComponent->GetRotation();

   const bool ret = (glm::length(pos - m_cachedPosition) > 0.01f) || (glm::length(rot - m_cachedRotation) > 0.01f);

   m_cachedPosition = pos;
   m_cachedRotation = rot;

   return ret;
}

void TransformReplicationUnit::Write(OutputMemoryBitStream& stream)
{
   Serialize(stream, m_cachedPosition.x);
   Serialize(stream, m_cachedPosition.y);
   Serialize(stream, m_cachedPosition.z);

   Serialize(stream, m_cachedRotation.x);
   Serialize(stream, m_cachedRotation.y);
   Serialize(stream, m_cachedRotation.z);
}

void TransformReplicationUnit::Read(InputMemoryBitStream& stream)
{
   glm::vec3 pos;
   Deserialize(stream, pos.x);
   Deserialize(stream, pos.y);
   Deserialize(stream, pos.z);

   m_pTransformComponent->SetPosition(pos);

   glm::vec3 rot;
   Deserialize(stream, rot.x);
   Deserialize(stream, rot.y);
   Deserialize(stream, rot.z);

   m_pTransformComponent->SetRotation(rot);
}

} // namespace BIEngine
