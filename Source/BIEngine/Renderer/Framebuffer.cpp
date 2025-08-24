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

SharedPtr<Framebuffer> GetDefaultFramebuffer()
{
   SharedPtr<Framebuffer> defFramebuffer = MakeShared<Framebuffer>();
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
      case FramebufferAttachementType::STENCIL:
         return GL_STENCIL_ATTACHMENT;
      case FramebufferAttachementType::DEPTH_STENCIL:
         return GL_DEPTH_STENCIL_ATTACHMENT;
      default:
         return 0;
   }
}

void FramebufferEnableColor(SharedPtr<Framebuffer> framebuffer, FramebufferColorOperationType op)
{
   framebuffer->Bind();

   if (static_cast<bool>(op & FramebufferColorOperationType::DRAW)) {
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
   }

   if (static_cast<bool>(op & FramebufferColorOperationType::READ)) {
      glReadBuffer(GL_COLOR_ATTACHMENT0);
   }
}

void FramebufferDisableColor(SharedPtr<Framebuffer> framebuffer, FramebufferColorOperationType op)
{
   framebuffer->Bind();

   if (static_cast<bool>(op & FramebufferColorOperationType::DRAW)) {
      glDrawBuffer(GL_NONE);
   }

   if (static_cast<bool>(op & FramebufferColorOperationType::READ)) {
      glReadBuffer(GL_NONE);
   }
}

void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Texture2D> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture2D(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_TEXTURE_2D, attachement->GetId(), 0);
}

void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Texture2DMultisample> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture2D(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_TEXTURE_2D_MULTISAMPLE, attachement->GetId(), 0);
}

void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<CubemapTexture> attachement)
{
   framebuffer->Bind();
   glFramebufferTexture(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), attachement->GetId(), 0);
}

void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<CubemapTexture> attachement, int sideIndex)
{
   framebuffer->Bind();
   glFramebufferTexture2D(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_TEXTURE_CUBE_MAP_POSITIVE_X + sideIndex, attachement->GetId(), 0);
}

void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Renderbuffer> attachement)
{
   framebuffer->Bind();
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, FramebufferAttachementTypeToOpengl(type), GL_RENDERBUFFER, attachement->GetId());
}

void Blit(SharedPtr<Framebuffer> src, SharedPtr<Framebuffer> dest, int screenWidth, int screenHeight)
{
   glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_framebufferId);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->m_framebufferId);
   glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

} // namespace BIEngine
