#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "../EngineCore/Assert.h"

namespace BIEngine {

class Skeleton {
public:
   static constexpr int MAX_BONES = 100;

   struct BoneInfo {
      std::string name;
      glm::mat4 localTransform;
      glm::mat4 offset;
      std::vector<std::shared_ptr<BoneInfo>> children;
   };

public:
   Skeleton(std::shared_ptr<BoneInfo> pSkeletonRoot);

   void Update()
   {
      calculateBoneTransform(m_pSkeletonRoot, glm::mat4(1.0f));
   }

   const glm::mat4& GetBoneMatrix(const std::string& boneName) const
   {
      std::unordered_map<std::string, glm::mat4>::const_iterator itr = m_finalBoneMatrices.find(boneName);
      Assert(itr != m_finalBoneMatrices.cend(), "Cannot find bone with name %s", boneName.c_str());
      return itr->second;
   }

private:
   void calculateBoneTransform(std::shared_ptr<Skeleton::BoneInfo> node, glm::mat4 parentTransform)
   {
      const glm::mat4 globalTransformation = parentTransform * node->localTransform;

      m_finalBoneMatrices[node->name] = globalTransformation * node->offset;

      for (int i = 0; i < node->children.size(); i++) {
         calculateBoneTransform(node->children[i], globalTransformation);
      }
   }

private:
   std::shared_ptr<BoneInfo> m_pSkeletonRoot;

   std::unordered_map<std::string, glm::mat4> m_finalBoneMatrices;
};

} // namespace BIEngine
