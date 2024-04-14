#include "WorldRenderPass.h"

#include "../Renderer/Renderbuffer.h"
#include "../Renderer/ShadersLoader.h"
#include "../Utilities/DebugDraw.h"

namespace BIEngine {

bool WorldRenderPass::InitInternal()
{
   GraphicsRenderPass::InitInternal();

   m_intermediateFramebuffer = std::make_shared<Framebuffer>();
   m_colorIntermediateBuffer = Texture2D::Create(m_screenWidth, m_screenHeight, Texture::SizedFormat::RGB_16_F, Texture::Format::RGB, nullptr);
   m_depthStencilIntermediateRenderuffer = Renderbuffer::Create(m_screenWidth, m_screenHeight, Renderbuffer::Format::DEPTH24_STENCIL8);
   FramebufferAttach(m_intermediateFramebuffer, FramebufferAttachementType::COLOR, m_colorIntermediateBuffer);
   FramebufferAttach(m_intermediateFramebuffer, FramebufferAttachementType::DEPTH_STENCIL, m_depthStencilIntermediateRenderuffer);
   if (!m_intermediateFramebuffer->Check()) {
      return false;
   }

   m_multisamplingFramebuffer = std::make_shared<Framebuffer>();
   m_colorMultisampleBuffer = Texture2DMultisample::Create(m_screenWidth, m_screenHeight, Texture::SizedFormat::RGB_16_F, m_msaaSamples);
   m_depthStencilMultisampleRenderuffer = Renderbuffer::Create(m_screenWidth, m_screenHeight, Renderbuffer::Format::DEPTH24_STENCIL8, m_msaaSamples);
   FramebufferAttach(m_multisamplingFramebuffer, FramebufferAttachementType::COLOR, m_colorMultisampleBuffer);
   FramebufferAttach(m_multisamplingFramebuffer, FramebufferAttachementType::DEPTH_STENCIL, m_depthStencilMultisampleRenderuffer);
   if (!m_multisamplingFramebuffer->Check()) {
      return false;
   }

   const std::string commonPostProcessingShaderProgramPath = "effects/commonPostProcessing.sp";
   auto shaderPgrogramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPostProcessingShaderProgramPath)->GetExtra());
   m_pDefaultPostProcessor = std::make_shared<PostProcessor>(shaderPgrogramData->GetShaderProgram());

   DebugDraw::Init();

   return true;
}

void WorldRenderPass::PreRender(Scene* const pScene)
{
   GraphicsRenderPass::PreRender(pScene);

   pScene->GetRenderer()->SetRenderTarget(m_multisamplingFramebuffer);

   glViewport(0, 0, pScene->GetRenderer()->GetScreenWidth(), pScene->GetRenderer()->GetScreenHeight());
   static const ColorRgba CLEAR_COLOR = ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
   pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH | RenderDevice::ClearFlag::STENCIL, CLEAR_COLOR);
}

void WorldRenderPass::PostRender(Scene* const pScene)
{
   DebugDraw::Draw();

   Blit(m_multisamplingFramebuffer, m_intermediateFramebuffer, pScene->GetRenderer()->GetScreenWidth(), pScene->GetRenderer()->GetScreenHeight());

   pScene->GetRenderer()->SetRenderTarget(m_pRenderTarget);
   m_pDefaultPostProcessor->Use(pScene->GetRenderer().get(), m_colorIntermediateBuffer);

   GraphicsRenderPass::PostRender(pScene);
}

} // namespace BIEngine
