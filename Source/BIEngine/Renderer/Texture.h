#pragma once

#include <array>

namespace BIEngine
{

    class Texture
    {
    public:
        Texture();
        virtual ~Texture();

        //Сделаеть текстуру активной
        virtual void Bind(int textureIdx) const = 0;

        //Ширина изображения в пикселях
        virtual unsigned int GetWidth() const { return m_width; }
        //Высота изображения в пикселях
        virtual unsigned int GetHeight() const { return m_height; }

        //Задает формат цветов текстуры
        void SetInternalFormat(unsigned int format) { m_internalFormat = format; }
        //Отдает формат цветов текстуры
        unsigned int GetInternalFormat() const { return m_internalFormat; }
        //Задает формат цветов изображения
        void SetImageFormat(unsigned int format) { m_imageFormat = format; }
        //Отдает формат цветов изображения
        unsigned int GetImageFormat() const { return m_imageFormat; }

    protected:
        unsigned int m_id;

        //Размеры загруженного изображения в пикселях
        unsigned int m_width, m_height;

        //Формат текстуры
        unsigned int m_internalFormat;
        //Формат изображения
        unsigned int m_imageFormat;
    };


    class Texture2D : public Texture
    {
    public:
        Texture2D();

        Texture2D(const Texture2D& orig);
        Texture2D& operator=(const Texture2D& orig);

        Texture2D(Texture2D&& orig);
        Texture2D& operator=(Texture2D&& orig);

        void Generate(unsigned int width, unsigned int height, unsigned char* data);
        //Сделаеть текстуру активной
        virtual void Bind(int textureIdx) const override;

    private:
        unsigned int m_wrapS;
        unsigned int m_wrapT;
        unsigned int m_filterMin;
        unsigned int m_filterMax;
    };


    class CubemapTexture : public Texture
    {
    public:
        CubemapTexture();

        CubemapTexture(const CubemapTexture& orig);
        CubemapTexture& operator=(const CubemapTexture& orig);

        CubemapTexture(CubemapTexture&& orig);
        CubemapTexture& operator=(CubemapTexture&& orig);

        void Generate(unsigned int width, unsigned int height, const std::array<unsigned char*, 6>& data);
        //Сделаеть текстуру активной
        virtual void Bind(int textureIdx) const override;

    private:
        unsigned int m_wrapS;
        unsigned int m_wrapT;
        unsigned int m_wrapR;
        unsigned int m_filterMin;
        unsigned int m_filterMax;
    };

}