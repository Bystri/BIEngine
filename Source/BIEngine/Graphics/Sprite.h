#pragma once

#include <memory>

#include "../Renderer/Texture.h"
#include "MeshGeometryGenerator.h"
#include "Material.h"

namespace BIEngine {

class Sprite {
public:
   explicit Sprite(std::shared_ptr<Texture2D> pTexture);

   std::shared_ptr<Mesh> GetMesh() const { return m_pSpriteMesh; }

   std::shared_ptr<Texture2D> GetTexture() const { return m_pMaterial->GetDiffuseMap(); }

   void SetSprite(std::shared_ptr<Texture2D> texture) { m_pMaterial->SetDiffuseMap(texture); }

   void SetColor(const Color& color) { m_pMaterial->SetColor(color); }

   const Color& GetColor() const { return m_pMaterial->GetColor(); }

   std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pMaterial->GetShaderProgramPtr(); }

   const RenderState& GetRanderState() const { return m_pMaterial->GetRenderState(); }

   ShaderProgramState ConstructShaderProgramState() const { return m_pMaterial->ConstructShaderProgramState(); }

private:
   std::shared_ptr<Mesh> m_pSpriteMesh;
   std::shared_ptr<Material> m_pMaterial;
};

} // namespace BIEngine
