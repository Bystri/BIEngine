#include "Animation.h"

namespace BIEngine {

BoneAnimChannel* Animation::FindBoneChannel(const String& boneName)
{
   auto iter = std::find_if(m_boneChannels.Begin(), m_boneChannels.End(), [&](const BoneAnimChannel& Bone) {
      return Bone.GetBoneName() == boneName;
   });

   if (iter == m_boneChannels.End()) {
      return nullptr;
   }

   return &(*iter);
}

} // namespace BIEngine
