#include "Animator.h"

#include "AnimationPose.h"
#include "AnimationSampler.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

Animator::Animator(Actor* pRoot)
   : m_pRoot(pRoot), m_pAnimationSampler(nullptr), m_currentTime(0.0f)
{
}

void Animator::Update(float dt)
{
   if (m_pAnimationSampler) {
      AnimationPose pose;
      m_pAnimationSampler->CalculatePoseForActor(m_pRoot, pose, dt);
      CalculateActorTransform(pose, m_pRoot);
   }
}

void Animator::PlayAnimation(SharedPtr<Animation> pAnimation)
{
   m_pAnimationSampler = MakeShared<AnimationSampler>(pAnimation);
   m_currentTime = 0.0f;
}

void Animator::CalculateActorTransform(const AnimationPose& pose, Actor* pActor)
{
   const AnimationPoseBoneTranformInfo* const boneTransformInfo = pose.GetBoneTranfromInfo(pActor->GetName());

   if (boneTransformInfo) {
      SharedPtr<TransformComponent> pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
      
      glm::mat4 blendedTransform = glm::translate(glm::mat4(1.0f), boneTransformInfo->GetPos());
      blendedTransform *= glm::mat4(glm::normalize(boneTransformInfo->GetOrientation()));
      blendedTransform *= glm::scale(glm::mat4(1.0f), boneTransformInfo->GetScale());

      pTransformComponent->SetLocalTransformMatrix(blendedTransform);
   }

   for (const auto& child : pActor->GetChildren()) {
      CalculateActorTransform(pose, child.Get());
   }
}

} // namespace BIEngine
