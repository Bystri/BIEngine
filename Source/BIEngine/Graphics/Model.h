#pragma once

#include <memory>

#include "Material.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Texture.h"

namespace BIEngine {

class ModelMesh {
public:
   explicit ModelMesh(std::shared_ptr<Mesh> pMesh, std::shared_ptr<Material> pMaterial)
      : m_pMesh(pMesh), m_pMaterial(pMaterial) {}

   std::shared_ptr<Mesh> GetMesh() const
   {
      return m_pMesh;
   }

   std::shared_ptr<Material> GetMaterial() const
   {
      return m_pMaterial;
   }


private:
   std::shared_ptr<Mesh> m_pMesh;
   std::shared_ptr<Material> m_pMaterial;
};

class Model {
public:
   Model()
      : m_meshes() {}

   std::size_t AddModelMesh(std::shared_ptr<ModelMesh> pMesh)
   {
      m_meshes.push_back(pMesh);
      return m_meshes.size() - 1;
   }

   void RemoveModelMesh(std::size_t index)
   {
      if (m_meshes.size() <= index) {
         return;
      }

      m_meshes.erase(m_meshes.begin() + index);
   }

   const std::vector<std::shared_ptr<ModelMesh>>& GetMeshes() const { return m_meshes; }

private:
   std::vector<std::shared_ptr<ModelMesh>> m_meshes;
};

} // namespace BIEngine
