#pragma once

#include <array>

namespace BIEngine {

class Texture {
public:
   Texture();
   virtual ~Texture();

   unsigned int GetId() const { return m_id; }

   // �������� �������� ��������
   virtual void Bind(int textureIdx) const = 0;

   // ������ ����������� � ��������
   unsigned int GetWidth() const { return m_width; }

   // ������ ����������� � ��������
   unsigned int GetHeight() const { return m_height; }

   // ������ ������ ������ ��������
   void SetInternalFormat(unsigned int format) { m_internalFormat = format; }

   // ������ ������ ������ ��������
   unsigned int GetInternalFormat() const { return m_internalFormat; }

protected:
   unsigned int m_id;

   // ������� ������������ ����������� � ��������
   unsigned int m_width, m_height;

   // ������ ��������
   unsigned int m_internalFormat;
};

class Texture2D : public Texture {
public:
   Texture2D();

   Texture2D(const Texture2D& orig);
   Texture2D& operator=(const Texture2D& orig);

   Texture2D(Texture2D&& orig);
   Texture2D& operator=(Texture2D&& orig);

   void Generate(unsigned int width, unsigned int height, unsigned char* data);
   // �������� �������� ��������
   virtual void Bind(int textureIdx) const override;

   // ������ ������ ������ �����������
   void SetImageFormat(unsigned int format) { m_imageFormat = format; }

   // ������ ������ ������ �����������
   unsigned int GetImageFormat() const { return m_imageFormat; }

protected:
   unsigned int m_wrapS;
   unsigned int m_wrapT;
   unsigned int m_filterMin;
   unsigned int m_filterMax;

   // ������ �����������
   unsigned int m_imageFormat;
};

class Texture2DMultisample : public Texture {
public:
   Texture2DMultisample();

   Texture2DMultisample(const Texture2DMultisample& orig);
   Texture2DMultisample& operator=(const Texture2DMultisample& orig);

   Texture2DMultisample(Texture2DMultisample&& orig);
   Texture2DMultisample& operator=(Texture2DMultisample&& orig);

   void Generate(unsigned int width, unsigned int height, int multisamplesCount);
   // �������� �������� ��������
   virtual void Bind(int textureIdx) const override;

private:
   int m_multisamplesCount;
};

class CubemapTexture : public Texture2D {
public:
   CubemapTexture();

   CubemapTexture(const CubemapTexture& orig);
   CubemapTexture& operator=(const CubemapTexture& orig);

   CubemapTexture(CubemapTexture&& orig);
   CubemapTexture& operator=(CubemapTexture&& orig);

   void Generate(unsigned int width, unsigned int height, const std::array<unsigned char*, 6>& data);
   // �������� �������� ��������
   virtual void Bind(int textureIdx) const override;

private:
   unsigned int m_wrapR;
};

} // namespace BIEngine
