#pragma once

#include "GraphicsRenderPass.h"
#include "../Renderer/PostProcessor.h"

namespace BIEngine {

class WorldRenderPass : public GraphicsRenderPass {
public:
   WorldRenderPass(int screenWidth, int screenHeight, int msaaSamples)
      : GraphicsRenderPass(),
        m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_msaaSamples(msaaSamples),
        m_multisamplingFramebuffer(nullptr), m_colorMultisampleBuffer(nullptr), m_depthStencilMultisampleRenderuffer(nullptr),
        m_intermediateFramebuffer(nullptr), m_colorIntermediateBuffer(nullptr), m_depthStencilIntermediateRenderuffer(nullptr),
        m_pDefaultPostProcessor(nullptr), m_pRenderTarget(GetDefaultFramebuffer())
   {
   }

   void SetRenderTarget(SharedPtr<Framebuffer> pRenderTarget) { m_pRenderTarget = pRenderTarget; }

protected:
   virtual bool InitInternal();

   virtual void PreRender(Scene* const pScene, const GameTimer& gt) override;

   virtual void PostRender(Scene* const pScene, const GameTimer& gt) override;

private:
   int m_screenWidth;
   int m_screenHeight;
   const int m_msaaSamples;

   SharedPtr<Framebuffer> m_multisamplingFramebuffer;
   SharedPtr<Texture2DMultisample> m_colorMultisampleBuffer;
   SharedPtr<Renderbuffer> m_depthStencilMultisampleRenderuffer;

   SharedPtr<Framebuffer> m_intermediateFramebuffer;
   SharedPtr<Texture2D> m_colorIntermediateBuffer;
   SharedPtr<Renderbuffer> m_depthStencilIntermediateRenderuffer;

   SharedPtr<PostProcessor> m_pDefaultPostProcessor;
   SharedPtr<Framebuffer> m_pRenderTarget;
};

} // namespace BIEngine
