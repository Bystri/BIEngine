#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "../EngineCore/Assert.h"
#include "../StdLib/HashMap.h"
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
      DynamicArray<std::shared_ptr<BoneInfo>> children;
   };

public:
   Skeleton(std::shared_ptr<BoneInfo> pSkeletonRoot);

   void Update()
   {
      calculateBoneTransform(m_pSkeletonRoot, glm::mat4(1.0f));
   }

   const glm::mat4& GetBoneMatrix(const String& boneName) const
   {
      auto itr = m_finalBoneMatrices.Find(boneName);
      Assert(itr != m_finalBoneMatrices.CEnd(), "Cannot find bone with name %s", boneName.CStr());
      return itr->second;
   }

private:
   void calculateBoneTransform(std::shared_ptr<Skeleton::BoneInfo> node, glm::mat4 parentTransform)
   {
      const glm::mat4 globalTransformation = parentTransform * node->localTransform;

      m_finalBoneMatrices[node->name] = globalTransformation * node->offset;

      for (int i = 0; i < node->children.Size(); i++) {
         calculateBoneTransform(node->children[i], globalTransformation);
      }
   }

private:
   std::shared_ptr<BoneInfo> m_pSkeletonRoot;

   HashMap<String, glm::mat4> m_finalBoneMatrices;
};

} // namespace BIEngine
