#include "Texture.h"

#include <iostream>

#include <glad/glad.h>

namespace BIEngine {

/***************/
// Texture
/**************/

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
      case Texture2D::TextureWrap::REPEAT:
         return GL_REPEAT;
      default:
         return 0;
   }
}

std::shared_ptr<Texture2D> Texture2D::Create(unsigned int width, unsigned int height, Texture::Format internalFormat, unsigned char* data, CreationParams params)
{
   struct make_shared_enabler : public Texture2D {};

   std::shared_ptr<Texture2D> texture = std::make_shared<make_shared_enabler>();

   texture->m_width = width;
   texture->m_height = height;
   texture->m_internalFormat = internalFormat;

   // Создание текстуры
   glBindTexture(GL_TEXTURE_2D, texture->m_id);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX + static_cast<int>(internalFormat), width, height, 0, GL_STENCIL_INDEX + static_cast<int>(internalFormat), GL_BYTE + static_cast<int>(params.DataType), data);

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

std::shared_ptr<Texture2DMultisample> Texture2DMultisample::Create(unsigned int width, unsigned int height, Texture::Format internalFormat, int multisamplesCount)
{
   struct make_shared_enabler : public Texture2DMultisample {};

   std::shared_ptr<Texture2DMultisample> texture = std::make_shared<make_shared_enabler>();

   texture->m_width = width;
   texture->m_height = height;
   texture->m_internalFormat = internalFormat;
   texture->m_multisamplesCount = multisamplesCount;

   // Создание текстуры
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->m_id);
   glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamplesCount, GL_STENCIL_INDEX + static_cast<int>(internalFormat), width, height, GL_TRUE);

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
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, ConvertWrapToGl(params.WrapR));
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
