#include "Renderer.h"

#include "../Utilities/DebugDraw.h"

namespace BIEngine {

Renderer::Renderer()
   : m_screenWidth(-1), m_screenHeight(-1), m_renderDevice(), m_defaultFramebuffer(nullptr), m_multisamplingFramebuffer(nullptr)
{
}

bool Renderer::Init(int screenWidth, int screenHeight, int MsaaSamples)
{
   m_screenWidth = screenWidth;
   m_screenHeight = screenHeight;

   m_defaultFramebuffer = GetDefaultFramebuffer();
   m_multisamplingFramebuffer = ConstructMultisampleFramebuffer(screenWidth, screenHeight, MsaaSamples);

   if (m_multisamplingFramebuffer == nullptr) {
      return false;
   }

   m_renderDevice.Init();

   return true;
}

void Renderer::Clear(RenderDevice::ClearFlag flags, const Color& color)
{
   m_renderDevice.Clear(flags, color);
}

void Renderer::BeginFrame()
{
   m_multisamplingFramebuffer->Bind();
}

void Renderer::EndFrame()
{
   Blit(m_multisamplingFramebuffer, m_defaultFramebuffer, m_screenWidth, m_screenHeight);
}

void Renderer::DrawRenderCommand(RenderCommand& renderCommand)
{
   const RenderState& renderState = renderCommand.RenderState;
   m_renderDevice.SetDepthTest(renderState.DepthTest);
   m_renderDevice.SetDepthWrite(renderState.DepthWrite);
   m_renderDevice.SetDepthFunc(renderState.DepthCompare);
   m_renderDevice.SetBlend(renderState.Blend);
   m_renderDevice.SetBlendFunc(renderState.BlendSrc, renderState.BlendDst);
   m_renderDevice.SetBlendEquation(renderState.BlendEquation);
   m_renderDevice.SetCull(renderState.Cull);
   m_renderDevice.SetCullFace(renderState.CullFace);
   m_renderDevice.SetCullWindingOrder(renderState.CullWindingOrder);

   renderCommand.GetShaderProgramState().SetMatrix4("model", renderCommand.Transform);
   renderCommand.GetShaderProgramState().Use();

   glBindVertexArray(renderCommand.GetMeshPtr()->m_VAO);
   glDrawElements(GL_TRIANGLES, renderCommand.GetMeshPtr()->m_indices.size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);
}

} // namespace BIEngine
