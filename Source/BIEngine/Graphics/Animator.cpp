#include "Animator.h"

#include "../Actors/TransformComponent.h"

namespace BIEngine {

Animator::Animator(Actor* pRoot)
   : m_pRoot(pRoot), m_pCurrentAnimation(nullptr), m_currentTime(0.0f)
{
}

void Animator::Update(float dt)
{
   if (m_pCurrentAnimation) {
      m_currentTime += m_pCurrentAnimation->GetTicksPerSecond() * dt;

      const float notClampedTime = m_currentTime;
      m_currentTime = fmod(m_currentTime, m_pCurrentAnimation->GetDuration());
      calculateActorTransform(m_pRoot);

      if (!m_pCurrentAnimation->IsLooped() && notClampedTime >= m_pCurrentAnimation->GetDuration()) {
         m_pCurrentAnimation = nullptr;
      }
   }
}

void Animator::PlayAnimation(std::shared_ptr<Animation> pAnimation)
{
   m_pCurrentAnimation = pAnimation;
   m_currentTime = 0.0f;
}

void Animator::calculateActorTransform(Actor* pActor)
{
   BoneAnimChannel* const boneChannel = m_pCurrentAnimation->FindBoneChannel(pActor->GetName());

   if (boneChannel) {
      boneChannel->Update(m_currentTime);
      std::shared_ptr<TransformComponent> pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
      pTransformComponent->SetLocalTransformMatrix(boneChannel->GetLocalTransform());
   }

   for (const auto& child : pActor->GetChildren()) {
      calculateActorTransform(child.get());
   }
}

} // namespace BIEngine
