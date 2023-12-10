#include "Texture.h"

#include <iostream>

#include <glad/glad.h>

namespace BIEngine {

Texture::Texture()
   : m_width(0), m_height(0), m_internalFormat(GL_RGB), m_imageFormat(GL_RGB)
{
   glGenTextures(1, &m_id);
}

Texture::~Texture()
{
   glDeleteTextures(1, &m_id);
}

Texture2D::Texture2D()
   : Texture(), m_wrapS(GL_REPEAT), m_wrapT(GL_REPEAT), m_filterMin(GL_LINEAR), m_filterMax(GL_LINEAR)
{
   glGenTextures(1, &m_id);
}

Texture2D::Texture2D(const Texture2D& orig)
{
   // �������� ������ ��� ��������� ��������
   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = 0;
   m_height = 0;

   // ���� � �������� �������� ������/������ - ������ �� ������ �������� � ���� �� �����������
   if (orig.m_width != 0 && orig.m_height != 0) {
      // �������� ������ �������� �� ���������
      unsigned char* data = new unsigned char[m_width * m_height * 4];
      glBindTexture(GL_TEXTURE_2D, orig.m_id);
      glGetTexImage(GL_TEXTURE_2D, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

      // ������� �� ������ ������������� �������� �������
      glGenTextures(1, &m_id);
      Generate(m_width, m_height, data);
   }
}

Texture2D& Texture2D::operator=(const Texture2D& orig)
{
   if (this == &orig)
      return *this;

   // ��� ���������� ������ ����������� ������ � ��������(���� ��� ����). ������ ������� ��� ��������� �� ����, ��� ��� ���������� ����� �������� ���� ������������� � ������������

   // �������� ������ ��� ��������� ��������
   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = 0;
   m_height = 0;

   // ���� � �������� �������� ������/������ - ������ �� ������ �������� � ���� �� �����������
   if (orig.m_width != 0 && orig.m_height != 0) {
      // �������� ������ �������� �� ���������
      unsigned char* data = new unsigned char[m_width * m_height * 4];
      glBindTexture(GL_TEXTURE_2D, orig.m_id);
      glGetTexImage(GL_TEXTURE_2D, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

      // ������� �� ������ ������������� �������� �������
      Generate(m_width, m_height, data);
   }

   return *this;
}

Texture2D::Texture2D(Texture2D&& orig)
{
   // �������� ������ ��� ��������� ��������
   m_id = orig.m_id;

   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = orig.m_width;
   m_height = orig.m_height;

   // ������ id ������ ����, ��� ����������� �������� ���������
   orig.m_id = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& orig)
{
   if (this == &orig)
      return *this;

   glDeleteTextures(1, &m_id);

   // �������� ������ ��� ��������� ��������
   m_id = orig.m_id;

   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = orig.m_width;
   m_height = orig.m_height;

   // ������ id ������ ����, ��� ����������� �������� ���������
   orig.m_id = 0;

   return *this;
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
   m_width = width;
   m_height = height;

   // �������� ��������
   glBindTexture(GL_TEXTURE_2D, m_id);
   glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

   // ������ ��������� ��������
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);

   // ���������� ��������, ����� ��� �� ������������ � ���������� ������
   glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind(int textureIdx) const
{
   glActiveTexture(GL_TEXTURE0 + textureIdx);
   glBindTexture(GL_TEXTURE_2D, m_id);
}

CubemapTexture::CubemapTexture()
   : Texture(), m_wrapS(GL_CLAMP_TO_EDGE), m_wrapT(GL_CLAMP_TO_EDGE), m_wrapR(GL_CLAMP_TO_EDGE), m_filterMin(GL_LINEAR), m_filterMax(GL_LINEAR)
{
}

CubemapTexture::CubemapTexture(const CubemapTexture& orig)
{
   // �������� ������ ��� ��������� ��������
   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_wrapR = orig.m_wrapR;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = 0;
   m_height = 0;

   // ���� � �������� �������� ������/������ - ������ �� ������ �������� � ���� �� �����������
   if (orig.m_width != 0 && orig.m_height != 0) {
      // �������� ������ �������� �� ���������
      glBindTexture(GL_TEXTURE_2D, orig.m_id);

      std::array<unsigned char*, 6> data;
      for (int i = 0; i < 6; ++i) {
         data[i] = new unsigned char[m_width * m_height * 4];
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
      }

      // ������� �� ������ ������������� �������� �������
      glGenTextures(1, &m_id);
      Generate(m_width, m_height, data);
   }
}

CubemapTexture& CubemapTexture::operator=(const CubemapTexture& orig)
{
   if (this == &orig)
      return *this;

   // ��� ���������� ������ ����������� ������ � ��������(���� ��� ����). ������ ������� ��� ��������� �� ����, ��� ��� ���������� ����� �������� ���� ������������� � ������������

   // �������� ������ ��� ��������� ��������
   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_wrapR = orig.m_wrapR;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = 0;
   m_height = 0;

   // ���� � �������� �������� ������/������ - ������ �� ������ �������� � ���� �� �����������
   if (orig.m_width != 0 && orig.m_height != 0) {
      // �������� ������ �������� �� ���������
      glBindTexture(GL_TEXTURE_2D, orig.m_id);

      std::array<unsigned char*, 6> data;
      for (int i = 0; i < 6; ++i) {
         data[i] = new unsigned char[m_width * m_height * 4];
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
      }

      // ������� �� ������ ������������� �������� �������
      Generate(m_width, m_height, data);
   }

   return *this;
}

CubemapTexture::CubemapTexture(CubemapTexture&& orig)
{
   // �������� ������ ��� ��������� ��������
   m_id = orig.m_id;

   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_wrapR = orig.m_wrapR;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = orig.m_width;
   m_height = orig.m_height;

   // ������ id ������ ����, ��� ����������� �������� ���������
   orig.m_id = 0;
}

CubemapTexture& CubemapTexture::operator=(CubemapTexture&& orig)
{
   if (this == &orig)
      return *this;

   glDeleteTextures(1, &m_id);

   // �������� ������ ��� ��������� ��������
   m_id = orig.m_id;

   m_internalFormat = orig.m_internalFormat;
   m_imageFormat = orig.m_imageFormat;

   m_wrapS = orig.m_wrapS;
   m_wrapT = orig.m_wrapT;
   m_wrapR = orig.m_wrapR;
   m_filterMin = orig.m_filterMin;
   m_filterMax = orig.m_filterMax;

   m_width = orig.m_width;
   m_height = orig.m_height;

   // ������ id ������ ����, ��� ����������� �������� ���������
   orig.m_id = 0;

   return *this;
}

void CubemapTexture::Generate(unsigned int width, unsigned int height, const std::array<unsigned char*, 6>& data)
{
   m_width = width;
   m_height = height;

   // �������� ��������
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

   for (int i = 0; i < data.size(); ++i) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
   }

   // ������ ��������� ��������
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_wrapS);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_wrapT);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_wrapR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_filterMin);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_filterMax);

   // ���������� ��������, ����� ��� �� ������������ � ���������� ������
   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubemapTexture::Bind(int textureIdx) const
{
   glActiveTexture(GL_TEXTURE0 + textureIdx);
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
} // namespace BIEngine