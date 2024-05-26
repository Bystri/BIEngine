#pragma once

#include <array>
#include <memory>

namespace BIEngine {

class Texture {
public:
   enum class SizedFormat {
      STENCIL_INDEX,
      DEPTH_COMPONENT,
      RED,
      GREEN,
      BLUE,
      ALPHA,
      RGB,
      RGBA,
      R_8_F,
      R_16_F,
      R_32_F,
      RG_16_F,
      RG_32_F,
      RGB_16_F,
      RGB_32_F,
      RGBA_16_F,
      RGBA_32_F,
      R_8_I,
      R_16_I,
      R_32_I,
      RG_8_I,
      RG_16_I,
      RG_32_I,
      RGB_8_I,
      RGB_16_I,
      RGB_32_I,
      RGBA_8_I,
      RGBA_16_I,
      RGBA_32_I,
      R_8_UI,
      R_16_UI,
      R_32_UI,
      RG_8_UI,
      RG_16_UI,
      RG_32_UI,
      RGB_8_UI,
      RGB_16_UI,
      RGB_32_UI,
      RGBA_8_UI,
      RGBA_16_UI,
      RGBA_32_UI
   };

   enum class Format {
      STENCIL_INDEX,
      DEPTH_COMPONENT,
      RED,
      GREEN,
      BLUE,
      ALPHA,
      RGB,
      RGBA
   };

   Texture();
   virtual ~Texture();

   unsigned int GetId() const { return m_id; }

   // Сделаеть текстуру активной
   virtual void Bind(int textureIdx) const = 0;

   // Ширина изображения в пикселях
   unsigned int GetWidth() const { return m_width; }

   // Высота изображения в пикселях
   unsigned int GetHeight() const { return m_height; }

protected:
   unsigned int m_id;

   // Размеры загруженного изображения в пикселях
   unsigned int m_width, m_height;

   // Формат текстуры
   SizedFormat m_sizedFormat;
   Format m_internalFormat;
};

class Texture2D : public Texture {
public:
   enum class Type {
      BYTE,
      UNSIGNED_BYTE,
      SHORT,
      UNSIGNED_SHORT,
      INT,
      UNSIGNED_INT,
      FLOAT
   };

   enum class TextureWrap {
      CLAMP_TO_BORDER,
      CLAMP_TO_EDGE,
      REPEAT
   };

   enum class TextureFunction {
      NEAREST,
      LINEAR
   };

   struct CreationParams {
      Type DataType = Type::UNSIGNED_BYTE;
      TextureWrap WrapS = TextureWrap::REPEAT;
      TextureWrap WrapT = TextureWrap::REPEAT;
      TextureFunction FilterMin = TextureFunction::LINEAR;
      TextureFunction FilterMax = TextureFunction::LINEAR;
   };

   static std::shared_ptr<Texture2D> Create(unsigned int width, unsigned int height, Texture::SizedFormat sizedFormat, Texture::Format internalFormat, unsigned char* data, CreationParams params = CreationParams());

   // Сделаеть текстуру активной
   virtual void Bind(int textureIdx) const override;

private:
   Texture2D();

   Texture2D(const Texture2D& orig) = delete;
   Texture2D& operator=(const Texture2D& orig) = delete;
};

class Texture2DMultisample : public Texture {
public:
   static std::shared_ptr<Texture2DMultisample> Create(unsigned int width, unsigned int height, Texture::SizedFormat sizedFormat, int multisamplesCount);

   // Сделаеть текстуру активной
   virtual void Bind(int textureIdx) const override;

private:
   Texture2DMultisample();

   Texture2DMultisample(const Texture2DMultisample& orig) = delete;
   Texture2DMultisample& operator=(const Texture2DMultisample& orig) = delete;

private:
   int m_multisamplesCount;
};

class CubemapTexture : public Texture {
public:
   enum class Type {
      BYTE,
      UNSIGNED_BYTE,
      SHORT,
      UNSIGNED_SHORT,
      INT,
      UNSIGNED_INT,
      FLOAT
   };

   enum class TextureWrap {
      CLAMP_TO_BORDER,
      CLAMP_TO_EDGE,
      REPEAT
   };

   enum class TextureFunction {
      LINEAR,
      NEAREST
   };

   struct CreationParams {
      Type DataType = Type::UNSIGNED_BYTE;
      TextureWrap WrapS = TextureWrap::REPEAT;
      TextureWrap WrapT = TextureWrap::REPEAT;
      TextureWrap WrapR = TextureWrap::REPEAT;
      TextureFunction FilterMin = TextureFunction::LINEAR;
      TextureFunction FilterMax = TextureFunction::LINEAR;
   };

   static std::shared_ptr<CubemapTexture> Create(unsigned int width, unsigned int height, Texture::SizedFormat sizedFormat, Texture::Format internalFormat, const std::array<unsigned char*, 6>& data, CreationParams params = CreationParams());

   // Сделаеть текстуру активной
   virtual void Bind(int textureIdx) const override;

private:
   CubemapTexture();

   CubemapTexture(const CubemapTexture& orig) = delete;
   CubemapTexture& operator=(const CubemapTexture& orig) = delete;
};

} // namespace BIEngine
