#pragma once

#include <memory>

#include "../Renderer/Texture.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "Material.h"

namespace BIEngine {

class Sprite {
public:
   explicit Sprite(std::shared_ptr<Material> pMaterial);

   std::shared_ptr<Mesh> GetMesh() const { return m_pSpriteMesh; }

   std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pMaterial->GetShaderProgramPtr(); }

   const RenderState& GetRanderState() const { return m_pMaterial->GetRenderState(); }

   ShaderProgramState ConstructShaderProgramState() const { return m_pMaterial->ConstructShaderProgramState(); }

private:
   std::shared_ptr<Mesh> m_pSpriteMesh;
   std::shared_ptr<Material> m_pMaterial;
};

} // namespace BIEngine
