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
   RenderCommand(const Mesh* pMesh, std::shared_ptr<ShaderProgram> pShader)
      : pMesh(pMesh), m_pShaderProgramState(pShader), m_shaderProgramState(pShader)
   {
   }

   const Mesh* GetMeshPtr() const
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
   const Mesh* const pMesh;
   std::shared_ptr<ShaderProgram> m_pShaderProgramState;
   ShaderProgramState m_shaderProgramState;
};

class PostProcessor;
class Renderbuffer;

// Отвечает за отрисвоку спрайтов
class Renderer {
public:
   Renderer();
   Renderer(const Renderer& orig) = delete;
   Renderer& operator=(const Renderer& rhs) = delete;

   int GetScreenWidth() const { return m_screenWidth; }

   int GetScreenHeight() const { return m_screenHeight; }

   bool Init(int screenWidth, int screenHeight);

   RenderDevice& GetRenderDevice() { return m_renderDevice; }

   void SetRenderTarget(std::shared_ptr<Framebuffer> pRenderTarget)
   {
      m_pRenderTarget = pRenderTarget;
      m_pRenderTarget->Bind();
   }

   void Clear(RenderDevice::ClearFlag flags, const ColorRgba& color);
   void DrawRenderCommand(RenderCommand& renderCommand);

public:
   int m_screenWidth;
   int m_screenHeight;

   RenderDevice m_renderDevice;

   std::shared_ptr<Framebuffer> m_pRenderTarget;
};
} // namespace BIEngine
