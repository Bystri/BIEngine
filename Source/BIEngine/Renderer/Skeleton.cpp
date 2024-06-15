#include "Skeleton.h"

namespace BIEngine {

Skeleton::Skeleton(std::shared_ptr<BoneInfo> pSkeletonRoot)
   : m_pSkeletonRoot(pSkeletonRoot), m_finalBoneMatrices()
{
}

} // namespace BIEngine
