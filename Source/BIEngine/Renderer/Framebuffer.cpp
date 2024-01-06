#include "Framebuffer.h"

#include <glad/glad.h>

#include "Texture.h"

namespace BIEngine {

Framebuffer::~Framebuffer()
{
   if (m_framebufferId != 0) {
      glDeleteFramebuffers(1, &m_framebufferId);
      glDeleteRenderbuffers(1, &m_rbo);
   }
}

void Framebuffer::Bind() const
{
   glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
}

void Framebuffer::SetColorBufferAttachment(std::shared_ptr<Texture> pColorAttachment)
{
   m_pColorTexture = pColorAttachment;
   glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
   glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_pColorTexture->GetId(), 0);
}

void Framebuffer::SetDepthBufferAttachment(std::shared_ptr<Texture> pDepthAttachment)
{
   m_pDepthTexture = pDepthAttachment;
   glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_pDepthTexture->GetId(), 0);
}

std::shared_ptr<Framebuffer> GetDefaultFramebuffer()
{
   std::shared_ptr<Framebuffer> defFramebuffer = std::make_shared<Framebuffer>();
   defFramebuffer->m_framebufferId = 0;

   return defFramebuffer;
}

std::shared_ptr<Framebuffer> ConstructFramebuffer(int screenWidth, int screenHeight)
{
   std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>();

   glGenFramebuffers(1, &framebuffer->m_framebufferId);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_framebufferId);

   framebuffer->m_pColorTexture = Texture2D::Create(screenWidth, screenHeight, Texture2D::Format::RGB, nullptr);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->m_pColorTexture->GetId(), 0);

   Texture2D::CreationParams depthTexCreationParams = Texture2D::CreationParams();
   depthTexCreationParams.WrapS = Texture2D::TextureWrap::CLAMP_TO_BORDER;
   depthTexCreationParams.WrapT = Texture2D::TextureWrap::CLAMP_TO_BORDER;
   depthTexCreationParams.FilterMin = Texture2D::TextureFunction::NEAREST;
   depthTexCreationParams.FilterMax = Texture2D::TextureFunction::NEAREST;
   depthTexCreationParams.DataType = Texture2D::Type::FLOAT;

   framebuffer->m_pDepthTexture = Texture2D::Create(screenWidth, screenHeight, Texture2D::Format::DEPTH_COMPONENT, nullptr, depthTexCreationParams);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->m_pDepthTexture->GetId(), 0);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      return nullptr;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return framebuffer;
}

std::shared_ptr<Framebuffer> ConstructMultisampleFramebuffer(int screenWidth, int screenHeight, int multisamplesCount)
{
   std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>();

   glGenFramebuffers(1, &framebuffer->m_framebufferId);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_framebufferId);

   framebuffer->m_pColorTexture = Texture2DMultisample::Create(screenWidth, screenHeight, Texture2D::Format::RGB, multisamplesCount);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->m_pColorTexture->GetId(), 0);

   framebuffer->m_pDepthTexture = Texture2DMultisample::Create(screenWidth, screenHeight, Texture2D::Format::DEPTH_COMPONENT, multisamplesCount);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->m_pDepthTexture->GetId(), 0);

   glGenRenderbuffers(1, &framebuffer->m_rbo);
   glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->m_rbo);
   glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisamplesCount, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
   glBindRenderbuffer(GL_RENDERBUFFER, 0);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->m_rbo);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      return nullptr;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return framebuffer;
}

void Blit(std::shared_ptr<Framebuffer> src, std::shared_ptr<Framebuffer> dest, int screenWidth, int screenHeight)
{
   glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_framebufferId);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->m_framebufferId);
   glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

} // namespace BIEngine
