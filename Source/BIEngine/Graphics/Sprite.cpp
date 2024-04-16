#include "Sprite.h"

#include "../Renderer/ShadersLoader.h"

namespace BIEngine {

Sprite::Sprite(std::shared_ptr<Material> pMaterial)
   : m_pSpriteMesh(std::make_shared<Mesh>(MeshGeometryGenerator::CreateQuad(1.0, 1.0))), m_pMaterial(pMaterial)
{
}

} // namespace BIEngine
