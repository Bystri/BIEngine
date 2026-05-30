#include "AnimationSampler.h"

namespace BIEngine {

void AnimationSampler::CalculatePoseForActor(Actor* pRoot, AnimationPose& pose, float dt)
{
   if (m_pCurrentAnimation) {
      m_currentTime += m_pCurrentAnimation->GetTicksPerSecond() * dt;

      if (m_currentTime >= m_pCurrentAnimation->GetDuration()) {
         if (m_pCurrentAnimation->IsLooped()) {
            m_currentTime -= m_pCurrentAnimation->GetDuration();
         } else {
            m_currentTime = m_pCurrentAnimation->GetDuration();
         }
      }
   }

   CalculateAnimationPose(pose, pRoot);
}

void AnimationSampler::CalculateAnimationPose(AnimationPose& pose, Actor* pActor)
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

} // namespace BIEngine