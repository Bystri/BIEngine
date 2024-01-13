#include "Renderer.h"

#include "../Utilities/DebugDraw.h"
#include "ShadersLoader.h"
#include "PostProcessor.h"
#include "Renderbuffer.h"

namespace BIEngine {

Renderer::Renderer()
   : m_screenWidth(-1), m_screenHeight(-1), m_renderDevice(),
     m_multisamplingFramebuffer(nullptr), m_colorMultisampleBuffer(nullptr), m_depthMultisampleRenderuffer(nullptr),
     m_intermediateFramebuffer(nullptr), m_colorIntermediateBuffer(nullptr), m_depthIntermediateRenderuffer(nullptr),
     m_pDefaultPostProcessor(nullptr)
{
}

bool Renderer::Init(int screenWidth, int screenHeight, int MsaaSamples)
{
   m_screenWidth = screenWidth;
   m_screenHeight = screenHeight;

   m_intermediateFramebuffer = std::make_shared<Framebuffer>();
   m_colorIntermediateBuffer = Texture2D::Create(screenWidth, screenHeight, Texture::Format::RGB, nullptr);
   m_depthIntermediateRenderuffer = Renderbuffer::Create(screenWidth, screenHeight, Renderbuffer::Format::DEPTH24);
   FramebufferAttach(m_intermediateFramebuffer, FramebufferAttachementType::COLOR, m_colorIntermediateBuffer);
   FramebufferAttach(m_intermediateFramebuffer, FramebufferAttachementType::DEPTH, m_depthIntermediateRenderuffer);
   if (!m_intermediateFramebuffer->Check()) {
      return false;
   }

   m_multisamplingFramebuffer = std::make_shared<Framebuffer>();
   m_colorMultisampleBuffer = Texture2DMultisample::Create(screenWidth, screenHeight, Texture::Format::RGB, MsaaSamples);
   m_depthIntermediateRenderuffer = Renderbuffer::Create(screenWidth, screenHeight, Renderbuffer::Format::DEPTH24, MsaaSamples);
   FramebufferAttach(m_multisamplingFramebuffer, FramebufferAttachementType::COLOR, m_colorMultisampleBuffer);
   FramebufferAttach(m_multisamplingFramebuffer, FramebufferAttachementType::DEPTH, m_depthIntermediateRenderuffer);
   if (!m_multisamplingFramebuffer->Check()) {
      return false;
   }

   m_renderDevice.Init();

   const std::string commonPostProcessingShaderProgramPath = "effects/commonPostProcessing.sp";
   auto shaderPgrogramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPostProcessingShaderProgramPath)->GetExtra());
   m_pDefaultPostProcessor = std::make_shared<PostProcessor>(shaderPgrogramData->GetShaderProgram(), GetDefaultFramebuffer());

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
   Blit(m_multisamplingFramebuffer, m_intermediateFramebuffer, m_screenWidth, m_screenHeight);

   m_renderDevice.SetDepthTest(false);
   m_renderDevice.SetCull(false);

   m_pDefaultPostProcessor->Use(m_colorIntermediateBuffer);
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

   glBindVertexArray(renderCommand.GetMeshPtr()->GetVao());
   glDrawElements(GL_TRIANGLES, renderCommand.GetMeshPtr()->GetIndices().size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);
}

} // namespace BIEngine
