#pragma once

#include <memory>

#include "Mesh.h"

namespace BIEngine {

class ShaderProgram;
class Framebuffer;

class PostProcessor {
public:
   PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram);

   void Use(std::shared_ptr<Texture2D> pTextureToProcess, std::shared_ptr<Framebuffer> pRenderTarget);

private:
   const Mesh m_screenOutputMesh;
   const std::shared_ptr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine
