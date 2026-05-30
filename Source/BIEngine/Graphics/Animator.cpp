#include "Animator.h"

#include "AnimationPose.h"
#include "AnimationBlender.h"
#include "AnimationSampler.h"
#include "../Actors/TransformComponent.h"

namespace BIEngine {

Animator::Animator(Actor* pRoot)
   : m_pRoot(pRoot)
{
}

void Animator::Update(float dt)
{
   if (m_pMainSampler && m_pSecondarySampler == nullptr) {
      AnimationPose mainPose;
      m_pMainSampler->CalculatePoseForActor(m_pRoot, mainPose, dt);
      CalculateActorTransform(mainPose, m_pRoot);
   }

   if (m_pMainSampler && m_pSecondarySampler) {
      AnimationPose mainPose;
      AnimationPose secondaryPose;

      m_pMainSampler->CalculatePoseForActor(m_pRoot, mainPose, dt);
      m_pSecondarySampler->CalculatePoseForActor(m_pRoot, secondaryPose, dt);

      AnimationBlender blender;
      AnimationPose result;
      blender.BlendPoses(m_pRoot, mainPose, secondaryPose, m_blendWeight, result);
      CalculateActorTransform(result, m_pRoot);

      m_blendWeight += 2.0f * dt;
      if (m_blendWeight >= 1.0f) {
         m_pSecondarySampler = nullptr;
      }
   }
}

void Animator::PlayAnimation(SharedPtr<Animation> pAnimation)
{
   if (m_pMainSampler == nullptr) {
      m_pMainSampler = MakeShared<AnimationSampler>(pAnimation);
   } else {
      m_pSecondarySampler = m_pMainSampler;
      m_pMainSampler = MakeShared<AnimationSampler>(pAnimation);
      m_blendWeight = 0.0f;
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
