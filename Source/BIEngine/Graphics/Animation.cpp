#include "Animation.h"

namespace BIEngine {

BoneAnimChannel* Animation::FindBoneChannel(const std::string& boneName)
{
   auto iter = std::find_if(m_boneChannels.begin(), m_boneChannels.end(), [&](const BoneAnimChannel& Bone) {
      return Bone.GetBoneName() == boneName;
   });

   if (iter == m_boneChannels.end()) {
      return nullptr;
   }

   return &(*iter);
}

} // namespace BIEngine
