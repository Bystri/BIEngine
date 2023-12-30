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

void Framebuffer::BindTexture() const
{
   m_pScreenTexture->Bind(0);
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

   std::shared_ptr<Texture2D> pScreenTexture = std::make_shared<Texture2D>();
   pScreenTexture->Generate(screenWidth, screenHeight, nullptr);
   framebuffer->m_pScreenTexture = pScreenTexture;
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->m_pScreenTexture->GetId(), 0);

   glGenRenderbuffers(1, &framebuffer->m_rbo);
   glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->m_rbo);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->m_rbo);

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

   std::shared_ptr<Texture2DMultisample> pScreenTexture = std::make_shared<Texture2DMultisample>();
   pScreenTexture->SetInternalFormat(GL_RGB);
   pScreenTexture->Generate(screenWidth, screenHeight, multisamplesCount);
   framebuffer->m_pScreenTexture = pScreenTexture;
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->m_pScreenTexture->GetId(), 0);

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
