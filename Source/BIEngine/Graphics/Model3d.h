#pragma once

#include <memory>

#include "Mesh.h"
#include "../Renderer/Texture.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine {

class Model3d {
public:
   explicit Model3d(std::shared_ptr<Mesh> pMesh)
      : m_pModelMesh(pMesh) {}

   std::shared_ptr<Mesh> GetMesh() const { return m_pModelMesh; }

private:
   std::shared_ptr<Mesh> m_pModelMesh;
};

} // namespace BIEngine