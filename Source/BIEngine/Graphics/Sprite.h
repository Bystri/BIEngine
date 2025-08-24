#pragma once

#include "../StdLib/SharedPtr.h"
#include "../Renderer/Texture.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "Material.h"

namespace BIEngine {

class Sprite {
public:
   explicit Sprite(SharedPtr<Material> pMaterial);

   SharedPtr<Mesh> GetMesh() const { return m_pSpriteMesh; }

   SharedPtr<ShaderProgram> GetShaderProgramPtr() { return m_pMaterial->GetShaderProgramPtr(); }

   const RenderState& GetRanderState() const { return m_pMaterial->GetRenderState(); }

   ShaderProgramState ConstructShaderProgramState() const { return m_pMaterial->ConstructShaderProgramState(); }

private:
   SharedPtr<Mesh> m_pSpriteMesh;
   SharedPtr<Material> m_pMaterial;
};

} // namespace BIEngine
