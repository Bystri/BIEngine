#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include "../Renderer/Skeleton.h"

namespace BIEngine {

class AnimationPoseBoneTranformInfo {
   friend class AnimationPose;
   friend class HashMap<String, AnimationPoseBoneTranformInfo>;

public:
   const glm::vec3& GetPos() const
   {
      return m_position;
   }

   const glm::quat GetOrientation() const
   {
      return m_orientation;
   }

   const glm::vec3 GetScale() const
   {
      return m_scale;
   }

private:
   AnimationPoseBoneTranformInfo() = default;

private:
   glm::vec3 m_position = glm::vec3(0.0f);
   glm::quat m_orientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
   glm::vec3 m_scale = glm::vec3(0.0f);
};

class AnimationPose {
public:
   void SetBonePosition(const String& boneName, const glm::vec3 pos)
   {
      m_boneTransformInfos[boneName].m_position = pos;
   }

   void SetBoneOrientation(const String& boneName, const glm::quat& quat)
   {
      m_boneTransformInfos[boneName].m_orientation = quat;
   }

   void SetBoneScale(const String& boneName, const glm::vec3 scale)
   {
      m_boneTransformInfos[boneName].m_scale = scale;
   }

   const AnimationPoseBoneTranformInfo* const GetBoneTranfromInfo(const String& boneName) const
   {
      auto itr = m_boneTransformInfos.Find(boneName);
      if (itr == m_boneTransformInfos.CEnd()) {
         return nullptr;
      }

      return &itr->second;
   }

private:
   HashMap<String, AnimationPoseBoneTranformInfo> m_boneTransformInfos;
};

} // namespace BIEngine