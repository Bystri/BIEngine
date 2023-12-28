#pragma once

#include <memory>

#include "Mesh.h"
#include "Color.h"
#include "RenderState.h"
#include "Framebuffer.h"
#include "ShaderProgram.h"
#include "ShaderProgramState.h"

namespace BIEngine {

struct RenderCommand {
   RenderCommand(std::shared_ptr<Mesh> pMesh, std::shared_ptr<ShaderProgram> pShader)
      : pMesh(pMesh), m_shaderProgramState(pShader)
   {
   }

   std::shared_ptr<Mesh> GetMeshPtr() const
   {
      return pMesh;
   }

   ShaderProgramState& GetShaderProgramState()
   {
      return m_shaderProgramState;
   }

   glm::mat4 Transform = glm::mat4(1.0f);

   RenderState RenderState{};

private:
   const std::shared_ptr<Mesh> pMesh;
   ShaderProgramState m_shaderProgramState;
};

// Отвечает за отрисвоку спрайтов
class Renderer {
public:
   Renderer();
   Renderer(const Renderer& orig) = delete;
   Renderer& operator=(const Renderer& rhs) = delete;

   bool Init(int screenWidth, int screenHeight, int MsaaSamples = 4);

   RenderDevice& GetRenderDevice() { return m_renderDevice; }

   void BeginFrame();
   void EndFrame();

   void Clear(RenderDevice::ClearFlag flags, const Color& color);
   void DrawRenderCommand(RenderCommand& renderCommand);

private:
   int m_screenWidth;
   int m_screenHeight;

   RenderDevice m_renderDevice;

   std::shared_ptr<Framebuffer> m_defaultFramebuffer;
   std::shared_ptr<Framebuffer> m_multisamplingFramebuffer;
};
} // namespace BIEngine
