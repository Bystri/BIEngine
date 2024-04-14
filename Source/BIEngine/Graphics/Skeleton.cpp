#include "Skeleton.h"

namespace BIEngine {

Skeleton::Skeleton(std::shared_ptr<BoneInfo> pSkeletonRoot)
   : m_pSkeletonRoot(pSkeletonRoot), m_finalBoneMatrices(MAX_BONES, glm::mat4(1.0f))
{
}

} // namespace BIEngine
