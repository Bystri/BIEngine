#pragma once

#include "Material.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Texture.h"
#include "../StdLib/SharedPtr.h"

namespace BIEngine {

class ModelMesh {
public:
   explicit ModelMesh(SharedPtr<Mesh> pMesh, SharedPtr<Material> pMaterial)
      : m_pMesh(pMesh), m_pMaterial(pMaterial) {}

   SharedPtr<Mesh> GetMesh() const
   {
      return m_pMesh;
   }

   SharedPtr<Material> GetMaterial() const
   {
      return m_pMaterial;
   }


private:
   SharedPtr<Mesh> m_pMesh;
   SharedPtr<Material> m_pMaterial;
};

class Model {
public:
   Model()
      : m_meshes() {}

   SizeT AddModelMesh(SharedPtr<ModelMesh> pMesh)
   {
      m_meshes.PushBack(pMesh);
      return m_meshes.Size() - 1;
   }

   void RemoveModelMesh(SizeT index)
   {
      if (m_meshes.Size() <= index) {
         return;
      }

      m_meshes.Erase(m_meshes.Begin() + index);
   }

   const DynamicArray<SharedPtr<ModelMesh>>& GetMeshes() const { return m_meshes; }

private:
   DynamicArray<SharedPtr<ModelMesh>> m_meshes;
};

} // namespace BIEngine
