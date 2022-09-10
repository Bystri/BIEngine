#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
    Texture2D();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    //Сделаеть текстуру активной
    void Bind() const;

    //Ширина изображения в пикселях
    unsigned int GetWidth() const { return m_width; }
    //Высота изображения в пикселях
    unsigned int GetHeight() const { return m_height; }

    //Задает формат цветов текстуры
    void SetInternalFormat(unsigned int format) { m_internalFormat = format; }
    //Отдает формат цветов текстуры
    unsigned int GetInternalFormat() const { return m_internalFormat; }
    //Задает формат цветов изображения
    void SetImageFormat(unsigned int format) { m_imageFormat = format; }
    //Отдает формат цветов изображения
    unsigned int GetImageFormat() const { return m_imageFormat; }

private:
    unsigned int m_id;

    //Размеры загруженного изображения в пикселях
    unsigned int m_width, m_height; 

    //Формат текстуры
    unsigned int m_internalFormat;
    //Формат изображения
    unsigned int m_imageFormat;

  
    unsigned int m_wrapS;
    unsigned int m_wrapT;
    unsigned int m_filterMin;
    unsigned int m_filterMax;
};