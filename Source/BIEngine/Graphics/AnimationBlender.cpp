#include "AnimationBlender.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>


namespace BIEngine
{

void AnimationBlender::BlendPoses(Actor* pActor, const AnimationPose& posA, const AnimationPose& posB, float weightA, AnimationPose& result)
{
   const AnimationPoseBoneTranformInfo* const boneATransformInfo = posA.GetBoneTranfromInfo(pActor->GetName());
   const AnimationPoseBoneTranformInfo* const boneBTransformInfo = posB.GetBoneTranfromInfo(pActor->GetName());

   if (boneATransformInfo && boneBTransformInfo) {
      const glm::vec3 blendedTranslation = weightA * boneATransformInfo->GetPos() + (1.0f - weightA) * boneBTransformInfo->GetPos();
      const glm::vec3 blendedScale = weightA * boneATransformInfo->GetScale() + (1.0f - weightA) * boneBTransformInfo->GetScale();

      glm::quat blendedRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
      const glm::quat rotA = weightA * boneATransformInfo->GetOrientation();
      blendedRotation += (glm::dot(rotA, blendedRotation) < 0.0f) ? -rotA : rotA;

      const glm::quat rotB = (1.0f - weightA) * boneBTransformInfo->GetOrientation();
      blendedRotation += (glm::dot(rotB, blendedRotation) < 0.0f) ? -rotB : rotB;

      result.SetBonePosition(pActor->GetName(), blendedTranslation);
      result.SetBoneScale(pActor->GetName(), blendedScale);
      result.SetBoneOrientation(pActor->GetName(), blendedRotation);
   }

   for (const auto& child : pActor->GetChildren()) {
      BlendPoses(child.Get(), posA, posB, weightA, result);
   }
}

}