#include "Skeleton.h"

namespace BIEngine {

Skeleton::Skeleton(SharedPtr<BoneInfo> pSkeletonRoot)
   : m_pSkeletonRoot(pSkeletonRoot), m_finalBoneMatrices()
{
}

} // namespace BIEngine
