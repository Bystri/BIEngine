#include "Sprite.h"

#include "../Renderer/ShadersLoader.h"

namespace BIEngine {

Sprite::Sprite(SharedPtr<Material> pMaterial)
   : m_pSpriteMesh(MakeShared<Mesh>(MeshGeometryGenerator::CreateQuad(1.0, 1.0))), m_pMaterial(pMaterial)
{
}

} // namespace BIEngine
