#pragma once

#include <memory>
#include <map>

#include "Skeleton.h"
#include "Animator.h"
#include "Material.h"
#include "../Renderer/SkeletalMesh.h"
#include "../Renderer/Texture.h"

namespace BIEngine {

class SkeletalModelMesh {
public:
   explicit SkeletalModelMesh(std::shared_ptr<SkeletalMesh> pMesh, std::shared_ptr<Material> pMaterial)
      : m_pMesh(pMesh), m_pMaterial(pMaterial) {}

   std::shared_ptr<SkeletalMesh> GetSkeletalMesh() const
   {
      return m_pMesh;
   }

   std::shared_ptr<Material> GetMaterial() const
   {
      return m_pMaterial;
   }


private:
   std::shared_ptr<SkeletalMesh> m_pMesh;
   std::shared_ptr<Material> m_pMaterial;
};

class SkeletalModel {
public:
   SkeletalModel(std::shared_ptr<Skeleton> pSkeleton)
      : m_meshes(), m_pSkeleton(pSkeleton) {}

   std::size_t AddSkeletalModelMesh(std::shared_ptr<SkeletalModelMesh> pMesh)
   {
      m_meshes.push_back(pMesh);
      return m_meshes.size() - 1;
   }

   void RemoveSkeletalModelMesh(std::size_t index)
   {
      if (m_meshes.size() <= index) {
         return;
      }

      m_meshes.erase(m_meshes.begin() + index);
   }

   const std::vector<std::shared_ptr<SkeletalModelMesh>>& GetSkeletalMeshes() const { return m_meshes; }

   std::shared_ptr<Skeleton> GetSkeleton() const { return m_pSkeleton; }

private:
   std::vector<std::shared_ptr<SkeletalModelMesh>> m_meshes;
   std::shared_ptr<Skeleton> m_pSkeleton;
};

} // namespace BIEngine
