#pragma once

#include <memory>

namespace BIEngine {

class Renderbuffer {
public:
   enum class Format {
      R8,
      R16,
      R32,
      RG8,
      RG16,
      RG32,
      RGB8,
      RGB565,
      RGBA8,
      DEPTH16,
      DEPTH24,
      DEPTH24_STENCIL8
   };

   ~Renderbuffer();

   static std::shared_ptr<Renderbuffer> Create(int width, int height, Renderbuffer::Format type, unsigned int samples = 0);

   void Bind() const;
   unsigned int GetId() const;

private:
   Renderbuffer();

private:
   unsigned int rbo;
};

} // namespace BIEngine
