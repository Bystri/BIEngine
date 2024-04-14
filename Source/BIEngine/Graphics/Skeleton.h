#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace BIEngine {

class Skeleton {
public:
   static constexpr int MAX_BONES = 100;

   struct BoneInfo {
      int id;
      std::string name;
      glm::mat4 parentNodeTransform;
      glm::mat4 offset;
      std::vector<std::shared_ptr<BoneInfo>> children;
   };

public:
   Skeleton(std::shared_ptr<BoneInfo> pSkeletonRoot);

   void SetBoneTransfrom(int boneId, const glm::mat4& boneTrans) { m_finalBoneMatrices[boneId] = boneTrans; }

   std::shared_ptr<BoneInfo> GetSkeletonRoot() const { return m_pSkeletonRoot; }

   const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_finalBoneMatrices; }

private:
   std::shared_ptr<BoneInfo> m_pSkeletonRoot;

   std::vector<glm::mat4> m_finalBoneMatrices;
};

} // namespace BIEngine
