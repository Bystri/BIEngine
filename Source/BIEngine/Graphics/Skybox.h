#pragma once

#include <memory>

#include "../Graphics/Mesh.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture.h"

namespace BIEngine {

class Scene;

class Skybox {
public:
   Skybox(std::shared_ptr<CubemapTexture> pCubemapTexture, std::shared_ptr<ShaderProgram> pShaderProgram);

   bool OnRender(Scene* pScene);

private:
   const Mesh m_cubeMesh;
   std::shared_ptr<CubemapTexture> m_pCubemapTexture;
   std::shared_ptr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine