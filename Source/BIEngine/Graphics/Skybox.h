#pragma once

#include "../Renderer/Mesh.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture.h"
#include "../StdLib/SharedPtr.h"

namespace BIEngine {

class Scene;

class Skybox {
public:
   Skybox(SharedPtr<CubemapTexture> pCubemapTexture, SharedPtr<ShaderProgram> pShaderProgram);

   bool OnRender(Scene* pScene);

private:
   const Mesh m_cubeMesh;
   SharedPtr<CubemapTexture> m_pCubemapTexture;
   SharedPtr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine
