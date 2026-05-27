#include "Animator.h"

#include "AnimationPose.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

Animator::Animator(Actor* pRoot)
   : m_pRoot(pRoot), m_pCurrentAnimation(nullptr), m_currentTime(0.0f)
{
}

void Animator::Update(float dt)
{
   if (m_pCurrentAnimation) {
      m_currentTime += m_pCurrentAnimation->GetTicksPerSecond() * dt;

      if (!m_pCurrentAnimation->IsLooped() && m_currentTime >= m_pCurrentAnimation->GetDuration()) {
         m_pCurrentAnimation = nullptr;
         return;
      }

      m_currentTime = fmod(m_currentTime, m_pCurrentAnimation->GetDuration());

      AnimationPose pose;
      CalculateAnimationPose(pose, m_pRoot);
      CalculateActorTransform(pose, m_pRoot);
   }
}

void Animator::PlayAnimation(SharedPtr<Animation> pAnimation)
{
   m_pCurrentAnimation = pAnimation;
   m_currentTime = 0.0f;
}

void Animator::CalculateAnimationPose(AnimationPose& pose, Actor* pActor)
{
   BoneAnimChannel* const boneChannel = m_pCurrentAnimation->FindBoneChannel(pActor->GetName());

   if (boneChannel) {
      const BoneAnimTranformInfo boneTransformInfo = boneChannel->GetTransformAtTime(m_currentTime);

      pose.SetBonePosition(pActor->GetName(), boneTransformInfo.GetPos());
      pose.SetBoneOrientation(pActor->GetName(), boneTransformInfo.GetOrientation());
      pose.SetBoneScale(pActor->GetName(), boneTransformInfo.GetScale());
   }
   
   for (const auto& child : pActor->GetChildren()) {
      CalculateAnimationPose(pose, child.Get());
   }
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
