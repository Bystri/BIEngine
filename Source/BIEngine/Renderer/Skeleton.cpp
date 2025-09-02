#include "Skeleton.h"

#include "../StdLib/Assert.h"

namespace BIEngine {

Skeleton::Skeleton(SharedPtr<BoneInfo> pSkeletonRoot)
   : m_pSkeletonRoot(pSkeletonRoot), m_finalBoneMatrices()
{
}

void Skeleton::Update()
{
   calculateBoneTransform(m_pSkeletonRoot, glm::mat4(1.0f));
}

const glm::mat4& Skeleton::GetBoneMatrix(const String& boneName) const
{
   auto itr = m_finalBoneMatrices.Find(boneName);
   Assert(itr != m_finalBoneMatrices.CEnd(), "Cannot find bone with name %s", boneName.CStr());
   return itr->second;
}

void Skeleton::calculateBoneTransform(SharedPtr<Skeleton::BoneInfo> node, glm::mat4 parentTransform)
{
   const glm::mat4 globalTransformation = parentTransform * node->localTransform;

   m_finalBoneMatrices[node->name] = globalTransformation * node->offset;

   for (int i = 0; i < node->children.Size(); i++) {
      calculateBoneTransform(node->children[i], globalTransformation);
   }
}

} // namespace BIEngine
