#pragma once

#include <memory>

namespace BIEngine {

class Texture;
class Texture2D;
class Texture2DMultisample;
class CubemapTexture;
class Renderbuffer;

class Framebuffer {
   friend std::shared_ptr<Framebuffer> GetDefaultFramebuffer();
   friend void Blit(std::shared_ptr<Framebuffer>, std::shared_ptr<Framebuffer>, int, int);

public:
   Framebuffer();
   ~Framebuffer();

   void Bind() const;
   bool Check() const;

private:
   unsigned int m_framebufferId;
};

std::shared_ptr<Framebuffer> GetDefaultFramebuffer();

enum class FramebufferAttachementType {
   COLOR,
   DEPTH,
   STENCIL,
   DEPTH_STENCIL
};

void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Texture2D> attachement);
void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Texture2DMultisample> attachement);
void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<CubemapTexture> attachement);
void FramebufferAttach(std::shared_ptr<Framebuffer> framebuffer, FramebufferAttachementType type, std::shared_ptr<Renderbuffer> attachement);

void Blit(std::shared_ptr<Framebuffer> src, std::shared_ptr<Framebuffer> dest, int screenWidth, int screenHeight);

} // namespace BIEngine
