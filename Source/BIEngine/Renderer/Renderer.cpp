#include "Renderer.h"

#include "../Utilities/DebugDraw.h"
#include "ShadersLoader.h"
#include "PostProcessor.h"
#include "Renderbuffer.h"

namespace BIEngine {

Renderer::Renderer()
   : m_screenWidth(-1), m_screenHeight(-1), m_renderDevice()
{
}

bool Renderer::Init(int screenWidth, int screenHeight)
{
   m_screenWidth = screenWidth;
   m_screenHeight = screenHeight;

   m_renderDevice.Init();

   m_pRenderTarget = GetDefaultFramebuffer();

   return true;
}

void Renderer::Clear(RenderDevice::ClearFlag flags, const ColorRgba& color)
{
   m_renderDevice.Clear(flags, color);
}

void Renderer::DrawRenderCommand(RenderCommand& renderCommand)
{
   const RenderState& renderState = renderCommand.RenderState;
   m_renderDevice.SetDepthTest(renderState.DepthTest);
   m_renderDevice.SetDepthWrite(renderState.DepthWrite);
   m_renderDevice.SetDepthFunc(renderState.DepthCompare);
   m_renderDevice.SetStencilTest(renderState.StencilTest);
   m_renderDevice.SetStencilWrite(renderState.StencilWrite);
   m_renderDevice.SetStencilFunc(renderState.StencilAction, renderState.StencilFuncRefValue, renderState.StencilFuncMask);
   m_renderDevice.SetBlend(renderState.Blend);
   m_renderDevice.SetBlendFunc(renderState.BlendSrc, renderState.BlendDst);
   m_renderDevice.SetBlendEquation(renderState.BlendEquation);
   m_renderDevice.SetCull(renderState.Cull);
   m_renderDevice.SetCullFace(renderState.CullFace);
   m_renderDevice.SetCullWindingOrder(renderState.CullWindingOrder);

   renderCommand.GetShaderProgramState().SetMatrix4("model", renderCommand.Transform);
   renderCommand.GetShaderProgramState().Use();

   glBindVertexArray(renderCommand.GetMeshPtr()->GetVao());
   glDrawElements(GL_TRIANGLES, renderCommand.GetMeshPtr()->GetIndices().size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);
}

} // namespace BIEngine
