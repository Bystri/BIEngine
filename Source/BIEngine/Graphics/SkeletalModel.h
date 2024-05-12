#pragma once

#include <queue>
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

   void OnRender()
   {
      m_pMesh->OnRender();
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
      if (!m_freeIndicies.empty()) {
         auto idx = m_freeIndicies.front();
         m_freeIndicies.pop();
         m_meshes[idx] = pMesh;
         return idx;
      }

      m_meshes.push_back(pMesh);
      return m_meshes.size() - 1;
   }

   void RemoveSkeletalModelMesh(std::size_t index)
   {
      if (m_meshes.size() <= index) {
         return;
      }

      m_meshes[index] = nullptr;
      m_freeIndicies.push(index);
   }

   void OnRender()
   {
      for (auto& mesh : m_meshes) {
         mesh->OnRender();
      }
   }

   const std::vector<std::shared_ptr<SkeletalModelMesh>>& GetSkeletalMeshes() const { return m_meshes; }

   std::shared_ptr<Skeleton> GetSkeleton() const { return m_pSkeleton; }

private:
   std::vector<std::shared_ptr<SkeletalModelMesh>> m_meshes;
   std::queue<std::size_t> m_freeIndicies;
   std::shared_ptr<Skeleton> m_pSkeleton;
};

} // namespace BIEngine
