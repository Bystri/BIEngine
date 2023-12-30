#pragma once

#include <memory>

namespace BIEngine {

class Texture;

class Framebuffer {
   friend std::shared_ptr<Framebuffer> GetDefaultFramebuffer();
   friend std::shared_ptr<Framebuffer> ConstructFramebuffer(int, int);
   friend std::shared_ptr<Framebuffer> ConstructMultisampleFramebuffer(int, int, int);
   friend void Blit(std::shared_ptr<Framebuffer>, std::shared_ptr<Framebuffer>, int, int);

public:
   Framebuffer()
      : m_framebufferId(0), m_pScreenTexture(nullptr), m_rbo(0)
   {
   }

   ~Framebuffer();

   void Bind() const;
   void BindTexture() const;

private:
   unsigned int m_framebufferId;
   std::shared_ptr<Texture> m_pScreenTexture;
   unsigned int m_rbo;
};

std::shared_ptr<Framebuffer> GetDefaultFramebuffer();
std::shared_ptr<Framebuffer> ConstructFramebuffer(int screenWidth, int screenHeight);
std::shared_ptr<Framebuffer> ConstructMultisampleFramebuffer(int screenWidth, int screenHeight, int multisamplesCount);

void Blit(std::shared_ptr<Framebuffer> src, std::shared_ptr<Framebuffer> dest, int screenWidth, int screenHeight);

} // namespace BIEngine
