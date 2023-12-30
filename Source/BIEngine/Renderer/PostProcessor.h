#pragma once

#include <memory>

#include "Mesh.h"

namespace BIEngine {

class ShaderProgram;
class Framebuffer;

class PostProcessor {
public:
   PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram, std::shared_ptr<Framebuffer> pFramebuffer);

   void Use(std::shared_ptr<Framebuffer> pBufferToProcess);

private:
   const Mesh m_screenOutputMesh;
   const std::shared_ptr<ShaderProgram> m_pShaderProgram;
   const std::shared_ptr<Framebuffer> m_pFramebuffer;
};

} // namespace BIEngine
