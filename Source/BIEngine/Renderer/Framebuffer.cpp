#include "Framebuffer.h"

#include <glad/glad.h>

#include "Texture.h"
#include "Renderbuffer.h"

namespace BIEngine {

Framebuffer::Framebuffer()
   : m_framebufferId(0)
{
   glGenFramebuffers(1, &m_framebufferId);
}

Framebuffer::~Framebuffer()
{
   glDeleteFramebuffers(1, &m_framebufferId);
}

void Framebuffer::Bind() const
{
   static int lastFramebufferId = -1;

   if (m_framebufferId == lastFramebufferId) {
      return;
   }

   lastFramebufferId = m_framebufferId;

   glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
}

bool Framebuffer::Check() const
{
   Bind();
   return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

std::shared_ptr<Framebuffer> GetDefaultFramebuffer()
{
   std::shared_ptr<Framebuffer> defFramebuffer = std::make_shared<Framebuffer>();
   defFramebuffer->m_framebufferId = 0;

   return defFramebuffer;
}

static unsigned int FramebufferAttachementTypeToOpengl(FramebufferAttachementType type)
{
   switch (type) {
      case FramebufferAttachementType::COLOR:
         return GL_COLOR_ATTACHMENT0;
      case FramebufferAttachementType::DEPTH:
         return GL_DEPTH_ATTACHMENT;
      default:
         return 0;
   }
}

void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Texture2D> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture2D(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_TEXTURE_2D, attachement->GetId(), 0);
}

void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Texture2DMultisample> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture2D(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_TEXTURE_2D_MULTISAMPLE, attachement->GetId(), 0);
}

void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<CubemapTexture> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), attachement->GetId(), 0);
}

void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Renderbuffer> attachement)
{
   framebuffer->Bind();
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_RENDERBUFFER, attachement->GetId());
}

void Blit(std::shared_ptr<Framebuffer> src, std::shared_ptr<Framebuffer> dest, int screenWidth, int screenHeight)
{
   glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_framebufferId);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->m_framebufferId);
   glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

} // namespace BIEngine
