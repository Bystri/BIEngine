#pragma once

#include <memory>

#include "Mesh.h"

namespace BIEngine {

class ShaderProgram;
class Framebuffer;
class Renderer;

class PostProcessor {
public:
   PostProcessor(SharedPtr<ShaderProgram> pShaderProgram);

   void Use(Renderer* const pRenderer, SharedPtr<Texture2D> pTextureToProcess);

private:
   const Mesh m_screenOutputMesh;
   const SharedPtr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine
