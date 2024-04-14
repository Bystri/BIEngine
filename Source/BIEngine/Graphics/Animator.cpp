#include "Animator.h"

#include "SkeletalModel.h"

namespace BIEngine {

Animator::Animator(std::shared_ptr<SkeletalModel> pModel)
   : m_pModel(pModel), m_currentTime(0.0f), m_pCurrentAnimation(nullptr)
{
}

void Animator::Update(float dt)
{
   if (m_pCurrentAnimation) {
      m_currentTime += m_pCurrentAnimation->GetTicksPerSecond() * dt;
      m_currentTime = fmod(m_currentTime, m_pCurrentAnimation->GetDuration());
      calculateBoneTransform(m_pModel->GetSkeleton()->GetSkeletonRoot(), glm::mat4(1.0f));
   }
}

void Animator::PlayAnimation(std::shared_ptr<Animation> pAnimation)
{
   m_pCurrentAnimation = pAnimation;
   m_currentTime = 0.0f;
}

void Animator::calculateBoneTransform(std::shared_ptr<Skeleton::BoneInfo> node, glm::mat4 parentTransform)
{
   glm::mat4 nodeTransform = node->parentNodeTransform;

   BoneAnimChannel* const boneChannel = m_pCurrentAnimation->FindBoneChannel(node->name);

   if (boneChannel) {
      boneChannel->Update(m_currentTime);
      nodeTransform = boneChannel->GetLocalTransform();
   }

   glm::mat4 globalTransformation = parentTransform * nodeTransform;

   m_pModel->GetSkeleton()->SetBoneTransfrom(node->id, globalTransformation * node->offset);

   for (int i = 0; i < node->children.size(); i++) {
      calculateBoneTransform(node->children[i], globalTransformation);
   }
}

} // namespace BIEngine
