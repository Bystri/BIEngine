#include "Texture.h"

#include <iostream>

#include <glad/glad.h>

#include "../EngineCore/Assert.h"

namespace BIEngine {

/***************/
// Texture
/**************/

static unsigned int ConverSizedFormatToGl(Texture2D::SizedFormat format)
{
   if (format < Texture2D::SizedFormat::R_16_F) {
      return GL_STENCIL_INDEX + static_cast<unsigned int>(format);
   }

   switch (format) {
      case Texture2D::SizedFormat::R_16_F:
         return GL_R16F;
      case Texture2D::SizedFormat::R_32_F:
         return GL_R32F;
      case Texture2D::SizedFormat::RG_16_F:
         return GL_RG16F;
      case Texture2D::SizedFormat::RG_32_F:
         return GL_RG32F;
      case Texture2D::SizedFormat::RGB_16_F:
         return GL_RGB16F;
      case Texture2D::SizedFormat::RGB_32_F:
         return GL_RGB32F;
      case Texture2D::SizedFormat::RGBA_32_F:
         return GL_RGBA32F;
      case Texture2D::SizedFormat::RGBA_16_F:
         return GL_RGBA16F;
      case Texture2D::SizedFormat::R_8_I:
         return GL_R8I;
      case Texture2D::SizedFormat::R_16_I:
         return GL_R16I;
      case Texture2D::SizedFormat::R_32_I:
         return GL_R32I;
      case Texture2D::SizedFormat::RG_8_I:
         return GL_RG8I;
      case Texture2D::SizedFormat::RG_16_I:
         return GL_RG16I;
      case Texture2D::SizedFormat::RG_32_I:
         return GL_RG32I;
      case Texture2D::SizedFormat::RGB_8_I:
         return GL_RGB8I;
      case Texture2D::SizedFormat::RGB_16_I:
         return GL_RGB16I;
      case Texture2D::SizedFormat::RGB_32_I:
         return GL_RGB32I;
      case Texture2D::SizedFormat::RGBA_8_I:
         return GL_RGBA8I;
      case Texture2D::SizedFormat::RGBA_16_I:
         return GL_RGBA16I;
      case Texture2D::SizedFormat::RGBA_32_I:
         return GL_RGBA32I;
      case Texture2D::SizedFormat::R_8_UI:
         return GL_R8UI;
      case Texture2D::SizedFormat::R_16_UI:
         return GL_R16UI;
      case Texture2D::SizedFormat::R_32_UI:
         return GL_R32UI;
      case Texture2D::SizedFormat::RG_8_UI:
         return GL_RG8UI;
      case Texture2D::SizedFormat::RG_16_UI:
         return GL_RG16UI;
      case Texture2D::SizedFormat::RG_32_UI:
         return GL_RG32UI;
      case Texture2D::SizedFormat::RGB_8_UI:
         return GL_RGB8UI;
      case Texture2D::SizedFormat::RGB_16_UI:
         return GL_RGB16UI;
      case Texture2D::SizedFormat::RGB_32_UI:
         return GL_RGB32UI;
      case Texture2D::SizedFormat::RGBA_8_UI:
         return GL_RGBA8UI;
      case Texture2D::SizedFormat::RGBA_16_UI:
         return GL_RGBA16UI;
      case Texture2D::SizedFormat::RGBA_32_UI:
         return GL_RGBA32UI;
   }

   Assert(false, "Texture2D SizedFormat foesn't specified in ConverSizedFormatToGl");
   return 0;
}

Texture::Texture()
   : m_width(0), m_height(0), m_internalFormat(Format::RGB)
{
   glGenTextures(1, &m_id);
}

Texture::~Texture()
{
   glDeleteTextures(1, &m_id);
}

/***************/
// Texture2D
/**************/

Texture2D::Texture2D()
   : Texture()
{
}

static unsigned int ConvertWrapToGl(Texture2D::TextureWrap wrap)
{
   switch (wrap) {
      case Texture2D::TextureWrap::CLAMP_TO_BORDER:
         return GL_CLAMP_TO_BORDER;
      case Texture2D::TextureWrap::CLAMP_TO_EDGE:
         return GL_CLAMP_TO_EDGE;
      case Texture2D::TextureWrap::REPEAT:
         return GL_REPEAT;
      default:
         return 0;
   }
}

std::shared_ptr<Texture2D> Texture2D::Create(unsigned int width, unsigned int height, Texture::SizedFormat sizedFormat, Texture::Format internalFormat, unsigned char* data, CreationParams params)
{
   struct make_shared_enabler : public Texture2D {};

   std::shared_ptr<Texture2D> texture = std::make_shared<make_shared_enabler>();

   texture->m_width = width;
   texture->m_height = height;
   texture->m_sizedFormat = sizedFormat;
   texture->m_internalFormat = internalFormat;

   // Создание текстуры
   glBindTexture(GL_TEXTURE_2D, texture->m_id);
   glTexImage2D(GL_TEXTURE_2D, 0, ConverSizedFormatToGl(sizedFormat), width, height, 0, GL_STENCIL_INDEX + static_cast<int>(internalFormat), GL_BYTE + static_cast<int>(params.DataType), data);

   // Задаем параметры текстуры
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ConvertWrapToGl(params.WrapS));
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ConvertWrapToGl(params.WrapT));
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + static_cast<int>(params.FilterMin));
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + static_cast<int>(params.FilterMax));

   float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

   // Отвязываем текстуру, чтобы она не учавствовала в дальнейшей работе
   glBindTexture(GL_TEXTURE_2D, 0);

   return texture;
}

void Texture2D::Bind(int textureIdx) const
{
   glActiveTexture(GL_TEXTURE0 + textureIdx);
   glBindTexture(GL_TEXTURE_2D, m_id);
}

/**********************/
// Texture2DMultisample
/*********************/

Texture2DMultisample::Texture2DMultisample()
   : Texture()
{
}

std::shared_ptr<Texture2DMultisample> Texture2DMultisample::Create(unsigned int width, unsigned int height, Texture::SizedFormat sizedFormat, int multisamplesCount)
{
   struct make_shared_enabler : public Texture2DMultisample {};

   std::shared_ptr<Texture2DMultisample> texture = std::make_shared<make_shared_enabler>();

   texture->m_width = width;
   texture->m_height = height;
   texture->m_sizedFormat = sizedFormat;
   texture->m_multisamplesCount = multisamplesCount;

   // Создание текстуры
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->m_id);
   glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamplesCount, ConverSizedFormatToGl(sizedFormat), width, height, GL_TRUE);

   // Отвязываем текстуру, чтобы она не учавствовала в дальнейшей работе
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

   return texture;
}

void Texture2DMultisample::Bind(int textureIdx) const
{
   glActiveTexture(GL_TEXTURE0 + textureIdx);
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
}

/***************/
// CubemapTexture
/**************/

CubemapTexture::CubemapTexture()
   : Texture()
{
}

static unsigned int ConvertWrapToGl(CubemapTexture::TextureWrap wrap)
{
   switch (wrap) {
      case CubemapTexture::TextureWrap::CLAMP_TO_BORDER:
         return GL_CLAMP_TO_BORDER;
      case CubemapTexture::TextureWrap::CLAMP_TO_EDGE:
         return GL_CLAMP_TO_EDGE;
      case CubemapTexture::TextureWrap::REPEAT:
         return GL_REPEAT;
      default:
         return 0;
   }
}

std::shared_ptr<CubemapTexture> CubemapTexture::Create(unsigned int width, unsigned int height, Texture::Format internalFormat, const std::array<unsigned char*, 6>& data, CreationParams params)
{
   struct make_shared_enabler : public CubemapTexture {};

   std::shared_ptr<CubemapTexture> texture = std::make_shared<make_shared_enabler>();

   texture->m_width = width;
   texture->m_height = height;
   texture->m_internalFormat = internalFormat;

   // Создание текстуры
   glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_id);

   for (int i = 0; i < data.size(); ++i) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_STENCIL_INDEX + static_cast<int>(internalFormat), width, height, 0, GL_STENCIL_INDEX + static_cast<int>(internalFormat), GL_BYTE + static_cast<int>(params.DataType), data[i]);
   }

   // Задаем параметры текстуры
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, ConvertWrapToGl(params.WrapS));
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, ConvertWrapToGl(params.WrapT));
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, ConvertWrapToGl(params.WrapR));
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST + static_cast<int>(params.FilterMin));
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST + static_cast<int>(params.FilterMax));

   // Отвязываем текстуру, чтобы она не учавствовала в дальнейшей работе
   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

   return texture;
}

void CubemapTexture::Bind(int textureIdx) const
{
   glActiveTexture(GL_TEXTURE0 + textureIdx);
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
} // namespace BIEngine
