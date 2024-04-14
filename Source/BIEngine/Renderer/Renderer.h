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
   RenderCommand(unsigned int VAO, int indicesNum, std::shared_ptr<ShaderProgram> pShader)
      : m_VAO(VAO), m_indicesNum(indicesNum), m_pShaderProgram(pShader), m_shaderProgramState(pShader)
   {
   }

   unsigned int GetVAO() const
   {
      return m_VAO;
   }

   int GetIndicesNum() const
   {
      return m_indicesNum;
   }

   ShaderProgramState& GetShaderProgramState()
   {
      return m_shaderProgramState;
   }

   glm::mat4 Transform = glm::mat4(1.0f);

   RenderState RenderState{};

private:
   unsigned int m_VAO;
   int m_indicesNum;

   std::shared_ptr<ShaderProgram> m_pShaderProgram;
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
