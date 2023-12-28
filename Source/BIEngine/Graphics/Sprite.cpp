#include "Sprite.h"

#include "../Renderer/ShadersLoader.h"

namespace BIEngine {

const std::string SPRITE_SHADER_PROGRAM_PATH = "Effects/sprite.mp";

Sprite::Sprite(std::shared_ptr<Texture2D> pTexture)
   : m_pSpriteMesh(std::make_shared<Mesh>(MeshGeometryGenerator::CreateQuad(1.0, 1.0))), m_pMaterial(nullptr)
{
   std::shared_ptr<ShaderProgramData> pShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(SPRITE_SHADER_PROGRAM_PATH)->GetExtra());
   m_pMaterial = std::make_shared<Material>(pShaderProgramData->GetShaderProgram());
   m_pMaterial->SetDiffuseMap(pTexture);
}

} // namespace BIEngine
