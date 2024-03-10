#pragma once

#include "GraphicsRenderPass.h"
#include "../Renderer/PostProcessor.h"

namespace BIEngine {

class WorldRenderPass : public GraphicsRenderPass {
public:
   WorldRenderPass(int screenWidth, int screenHeight, int msaaSamples)
      : GraphicsRenderPass(),
        m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_msaaSamples(msaaSamples),
        m_multisamplingFramebuffer(nullptr), m_colorMultisampleBuffer(nullptr), m_depthMultisampleRenderuffer(nullptr),
        m_intermediateFramebuffer(nullptr), m_colorIntermediateBuffer(nullptr), m_depthIntermediateRenderuffer(nullptr),
        m_pDefaultPostProcessor(nullptr), m_pRenderTarget(GetDefaultFramebuffer())
   {
   }

   void SetRenderTarget(std::shared_ptr<Framebuffer> pRenderTarget) { m_pRenderTarget = pRenderTarget; }

protected:
   virtual bool InitInternal();

   virtual void PreRender(Scene* const pScene) override;

   virtual void PostRender(Scene* const pScene) override;

private:
   int m_screenWidth;
   int m_screenHeight;
   const int m_msaaSamples;

   std::shared_ptr<Framebuffer> m_multisamplingFramebuffer;
   std::shared_ptr<Texture2DMultisample> m_colorMultisampleBuffer;
   std::shared_ptr<Renderbuffer> m_depthMultisampleRenderuffer;

   std::shared_ptr<Framebuffer> m_intermediateFramebuffer;
   std::shared_ptr<Texture2D> m_colorIntermediateBuffer;
   std::shared_ptr<Renderbuffer> m_depthIntermediateRenderuffer;

   std::shared_ptr<PostProcessor> m_pDefaultPostProcessor;
   std::shared_ptr<Framebuffer> m_pRenderTarget;
};

} // namespace BIEngine
