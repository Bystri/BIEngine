#pragma once

#include <memory>

#include "Mesh.h"

namespace BIEngine {

class ShaderProgram;
class Framebuffer;
class Renderer;

class PostProcessor {
public:
   PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram);

   void Use(Renderer* const pRenderer, std::shared_ptr<Texture2D> pTextureToProcess);

private:
   const Mesh m_screenOutputMesh;
   const std::shared_ptr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine
