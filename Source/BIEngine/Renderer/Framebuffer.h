#pragma once

#include <memory>

namespace BIEngine {

class Texture;
class CubemapTexture;

class Framebuffer {
   friend std::shared_ptr<Framebuffer> GetDefaultFramebuffer();
   friend std::shared_ptr<Framebuffer> ConstructFramebuffer(int, int);
   friend std::shared_ptr<Framebuffer> ConstructFramebuffer(int, int, std::shared_ptr<CubemapTexture>);
   friend std::shared_ptr<Framebuffer> ConstructMultisampleFramebuffer(int, int, int);
   friend void Blit(std::shared_ptr<Framebuffer>, std::shared_ptr<Framebuffer>, int, int);

public:
   Framebuffer()
      : m_framebufferId(0), m_pColorTexture(nullptr), m_rbo(0)
   {
   }

   ~Framebuffer();

   void Bind() const;

   void SetColorBufferAttachment(std::shared_ptr<Texture> pColorAttachment);

   void SetDepthBufferAttachment(std::shared_ptr<Texture> pDepthAttachment);

   std::shared_ptr<Texture> GetColorBufferAttachment() const { return m_pColorTexture; }

   std::shared_ptr<Texture> GetDepthBufferAttachment() const { return m_pDepthTexture; }

private:
   unsigned int m_framebufferId;
   std::shared_ptr<Texture> m_pColorTexture;
   std::shared_ptr<Texture> m_pDepthTexture;
   unsigned int m_rbo;
};

std::shared_ptr<Framebuffer> GetDefaultFramebuffer();
std::shared_ptr<Framebuffer> ConstructFramebuffer(int screenWidth, int screenHeight);
std::shared_ptr<Framebuffer> ConstructMultisampleFramebuffer(int screenWidth, int screenHeight, int multisamplesCount);

void Blit(std::shared_ptr<Framebuffer> src, std::shared_ptr<Framebuffer> dest, int screenWidth, int screenHeight);

} // namespace BIEngine
