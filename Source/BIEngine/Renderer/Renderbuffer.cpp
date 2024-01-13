#include "Renderbuffer.h"

#include <glad/glad.h>

namespace BIEngine {

Renderbuffer::Renderbuffer()
{
   glGenRenderbuffers(1, &rbo);
}

Renderbuffer::~Renderbuffer()
{
   glDeleteRenderbuffers(1, &rbo);
}

void Renderbuffer::Bind() const
{
   glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

unsigned int Renderbuffer::GetId() const
{
   return rbo;
}

static unsigned int renderbufferFormatTypeToOpengl(Renderbuffer::Format type)
{
   switch (type) {
      case Renderbuffer::Format::R8:
         return GL_R8;
      case Renderbuffer::Format::R16:
         return GL_R16;
      case Renderbuffer::Format::R32:
         return GL_R32I;
      case Renderbuffer::Format::RG8:
         return GL_RG8;
      case Renderbuffer::Format::RG16:
         return GL_RG16;
      case Renderbuffer::Format::RG32:
         return GL_RG32I;
      case Renderbuffer::Format::RGB8:
         return GL_RGB8;
      case Renderbuffer::Format::RGB565:
         return GL_RGB565;
      case Renderbuffer::Format::RGBA8:
         return GL_RGBA8;
      case Renderbuffer::Format::DEPTH16:
         return GL_DEPTH_COMPONENT16;
      case Renderbuffer::Format::DEPTH24:
         return GL_DEPTH_COMPONENT24;
      case Renderbuffer::Format::DEPTH24_STENCIL8:
         return GL_DEPTH24_STENCIL8;
      default:
         return 0u;
   }
}

std::shared_ptr<Renderbuffer> Renderbuffer::Create(int width, int height, Renderbuffer::Format type, unsigned int samples)
{
   struct make_shared_enabler : public Renderbuffer {};

   std::shared_ptr<make_shared_enabler> renderbuffer = std::make_shared<make_shared_enabler>();
   renderbuffer->Bind();
   glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, renderbufferFormatTypeToOpengl(type), width, height);

   return renderbuffer;
}

} // namespace BIEngine
