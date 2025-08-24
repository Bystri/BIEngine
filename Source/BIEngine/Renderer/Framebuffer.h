#pragma once

#include "../StdLib/SharedPtr.h"

namespace BIEngine {

class Texture;
class Texture2D;
class Texture2DMultisample;
class CubemapTexture;
class Renderbuffer;

class Framebuffer {
   friend SharedPtr<Framebuffer> GetDefaultFramebuffer();
   friend void Blit(SharedPtr<Framebuffer>, SharedPtr<Framebuffer>, int, int);

public:
   Framebuffer();
   ~Framebuffer();

   void Bind() const;
   bool Check() const;

private:
   unsigned int m_framebufferId;
};

SharedPtr<Framebuffer> GetDefaultFramebuffer();

enum class FramebufferColorOperationType : unsigned char {
   NONE,
   DRAW = 1,
   READ = 1 << 1,
   ALL = DRAW | READ
};

inline FramebufferColorOperationType operator|(FramebufferColorOperationType lhs, FramebufferColorOperationType rhs)
{
   using T = std::underlying_type_t<FramebufferColorOperationType>;
   return static_cast<FramebufferColorOperationType>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline FramebufferColorOperationType& operator|=(FramebufferColorOperationType& lhs, FramebufferColorOperationType rhs)
{
   lhs = lhs | rhs;
   return lhs;
}

inline FramebufferColorOperationType operator&(FramebufferColorOperationType lhs, FramebufferColorOperationType rhs)
{
   using T = std::underlying_type_t<FramebufferColorOperationType>;
   return static_cast<FramebufferColorOperationType>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline FramebufferColorOperationType& operator&=(FramebufferColorOperationType& lhs, FramebufferColorOperationType rhs)
{
   lhs = lhs & rhs;
   return lhs;
}

enum class FramebufferAttachementType {
   COLOR,
   DEPTH,
   STENCIL,
   DEPTH_STENCIL
};

void FramebufferEnableColor(SharedPtr<Framebuffer> framebuffer, FramebufferColorOperationType op);
void FramebufferDisableColor(SharedPtr<Framebuffer> framebuffer, FramebufferColorOperationType op);
void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Texture2D> attachement);
void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Texture2DMultisample> attachement);
void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<CubemapTexture> attachement);
void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<CubemapTexture> attachement, int sideIndex);
void FramebufferAttach(SharedPtr<Framebuffer> framebuffer, FramebufferAttachementType type, SharedPtr<Renderbuffer> attachement);

void Blit(SharedPtr<Framebuffer> src, SharedPtr<Framebuffer> dest, int screenWidth, int screenHeight);

} // namespace BIEngine
