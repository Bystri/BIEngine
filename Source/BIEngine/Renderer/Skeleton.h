#pragma once

#include <glm/glm.hpp>

#include "../StdLib/HashMap.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/DynamicArray.h"
#include "../StdLib/String.h"

namespace BIEngine {

class Skeleton {
public:
   static constexpr int MAX_BONES = 100;

   struct BoneInfo {
      String name;
      glm::mat4 localTransform;
      glm::mat4 offset;
      DynamicArray<SharedPtr<BoneInfo>> children;
   };

public:
   Skeleton(SharedPtr<BoneInfo> pSkeletonRoot);

   void Update();

   const glm::mat4& GetBoneMatrix(const String& boneName) const;

private:
   void calculateBoneTransform(SharedPtr<Skeleton::BoneInfo> node, glm::mat4 parentTransform);

private:
   SharedPtr<BoneInfo> m_pSkeletonRoot;

   HashMap<String, glm::mat4> m_finalBoneMatrices;
};

} // namespace BIEngine
