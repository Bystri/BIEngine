#include "TransformReplicationUnit.h"

#include "../../Actors/Actor.h"
#include "../../../StdLib/Algorithm.h"
#include "../../../EngineCore/GameApp.h"
#include "../../../Network/NetworkManager.h"
#include "../../../ProcessManager/ProcessManager.h"
#include "../../../Network/Replication/ObjectReplication.h"

namespace BIEngine {

class InterpolationProcess : public Process {
public:
   InterpolationProcess(WeakPtr<TransformComponent> pTransformComponent, const glm::vec3 posEnd, float timeToTake)
      : m_pTransformComponent(pTransformComponent), m_posStart(pTransformComponent.Lock()->GetPosition()), m_posEnd(posEnd), m_timeToTake(timeToTake), m_duration(0.0f)
   {
   }

protected:
   virtual void OnUpdate(float dt) override
   {
      auto pTransformComponent = m_pTransformComponent.Lock();
      if (pTransformComponent == nullptr) {
         Fail();
         return;
      }

      m_duration += dt;
      const float fraction = m_duration / m_timeToTake;
      if (fraction >= 1.0f) {
         Succeed();
         return;
      }

      pTransformComponent->SetPosition(glm::mix(m_posStart, m_posEnd, fraction));
   }

   virtual void OnSuccess() override
   {
      auto pTransformComponent = m_pTransformComponent.Lock();
      if (pTransformComponent == nullptr) {
         return;
      }

      pTransformComponent->SetPosition(m_posEnd);
   }

protected:
   const WeakPtr<TransformComponent> m_pTransformComponent;
   const glm::vec3 m_posStart;
   const glm::vec3 m_posEnd;
   const float m_timeToTake;
   float m_duration;
};

void TransformReplicationUnit::Init(ReplicationObject* pRelicationObject, SharedPtr<Actor> pActor)
{
   ReplicationUnit::Init(pRelicationObject, pActor);
   m_pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
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

   auto pProcess = m_pInterpolationProcess.Lock();
   if (pProcess != nullptr && pProcess->IsAlive())
   {
      pProcess->Fail();
   }

   m_pInterpolationProcess = BIEngine::ProcessManager::Get()->AttachProcess(
       MakeShared<InterpolationProcess>(
           WeakPtr<TransformComponent>(m_pTransformComponent), 
           pos, 
           g_pApp->m_pGameLogic->GetNetworkManager()->GetRttForPeer(GetOwner()->GetMasterPeerId())
       ));

   glm::vec3 rot;
   Deserialize(stream, rot.x);
   Deserialize(stream, rot.y);
   Deserialize(stream, rot.z);

   m_pTransformComponent->SetRotation(rot);
}

} // namespace BIEngine
