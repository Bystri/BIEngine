#include "Animation.h"

#include "../StdLib/Algorithm.h"

namespace BIEngine {

BoneAnimChannel* Animation::FindBoneChannel(const String& boneName)
{
   auto iter = FindIf(m_boneChannels.Begin(), m_boneChannels.End(), [&](const BoneAnimChannel& Bone) {
      return Bone.GetBoneName() == boneName;
   });

   if (iter == m_boneChannels.End()) {
      return nullptr;
   }

   return &(*iter);
}

} // namespace BIEngine
