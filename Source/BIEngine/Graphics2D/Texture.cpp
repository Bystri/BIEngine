#include <iostream>

#include "texture.h"


Texture2D::Texture2D()
    : m_width(0), m_height(0), m_internalFormat(GL_RGB), m_imageFormat(GL_RGB), m_wrapS(GL_REPEAT), m_wrapT(GL_REPEAT), m_filterMin(GL_LINEAR), m_filterMax(GL_LINEAR)
{
    glGenTextures(1, &m_id);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

Texture2D::Texture2D(const Texture2D& orig)
{
    //Копируем данные для генерации текстуры
    m_internalFormat = orig.m_internalFormat;
    m_imageFormat = orig.m_imageFormat;

    m_wrapS = orig.m_wrapS;
    m_wrapT = orig.m_wrapT;
    m_filterMin = orig.m_filterMin;
    m_filterMax = orig.m_filterMax;

    m_width = 0;
    m_height = 0;

    //Если у оргинила ненулвые высота/ширина - значит он хранит текстуру и надо ее скопировать
    if (orig.m_width != 0 && orig.m_height != 0)
    {
        //Копируем данные текстуры из оригинала
        unsigned char* data = new unsigned char[m_width * m_height * 4];
        glBindTexture(GL_TEXTURE_2D, orig.m_id);
        glGetTexImage(GL_TEXTURE_2D, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

        //Создаем на основе скопированной текстуры текущую
        glGenTextures(1, &m_id);
        Generate(m_width, m_height, data);
    }
}

Texture2D& Texture2D::operator=(const Texture2D& orig)
{
    if (this == &orig)
        return *this;

    //НАМ ДОСТАТОЧНО ТОЛЬКО СКОПИРОВАТЬ ДАННЫЕ И ТЕКСТУРУ(ЕСЛИ ОНА ЕСТЬ). НИЧЕГО УДАЛЯТЬ ИЛИ СОЗДАВАТЬ НЕ НАДО, ТАК КАК АБСТРАКЦИЯ НАШЕЙ ТЕКСТУРЫ БЫЛА СГЕНЕРИРОВАНА В КОНСТРУКТОРЕ

    //Копируем данные для генерации текстуры
    m_internalFormat = orig.m_internalFormat;
    m_imageFormat = orig.m_imageFormat;

    m_wrapS = orig.m_wrapS;
    m_wrapT = orig.m_wrapT;
    m_filterMin = orig.m_filterMin;
    m_filterMax = orig.m_filterMax;

    m_width = 0;
    m_height = 0;

    //Если у оргинила ненулвые высота/ширина - значит он хранит текстуру и надо ее скопировать
    if (orig.m_width != 0 && orig.m_height != 0)
    {
        //Копируем данные текстуры из оригинала
        unsigned char* data = new unsigned char[m_width * m_height * 4];
        glBindTexture(GL_TEXTURE_2D, orig.m_id);
        glGetTexImage(GL_TEXTURE_2D, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

        //Создаем на основе скопированной текстуры текущую
        Generate(m_width, m_height, data);
    }

    return *this;
}

Texture2D::Texture2D(Texture2D&& orig)
{
    //Копируем данные для генерации текстуры
    m_id = orig.m_id;

    m_internalFormat = orig.m_internalFormat;
    m_imageFormat = orig.m_imageFormat;

    m_wrapS = orig.m_wrapS;
    m_wrapT = orig.m_wrapT;
    m_filterMin = orig.m_filterMin;
    m_filterMax = orig.m_filterMax;

    m_width = orig.m_width;
    m_height = orig.m_height;

    //Задаем id равный нулю, для безопасного удаления оригинала 
    orig.m_id = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& orig)
{
    if (this == &orig)
        return *this;

    glDeleteTextures(1, &m_id);

    //Копируем данные для генерации текстуры
    m_id = orig.m_id;

    m_internalFormat = orig.m_internalFormat;
    m_imageFormat = orig.m_imageFormat;

    m_wrapS = orig.m_wrapS;
    m_wrapT = orig.m_wrapT;
    m_filterMin = orig.m_filterMin;
    m_filterMax = orig.m_filterMax;

    m_width = orig.m_width;
    m_height = orig.m_height;

    //Задаем id равный нулю, для безопасного удаления оригинала 
    orig.m_id = 0;

    return *this;
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    m_width = width;
    m_height = height;

    // Создание текстуры
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

    //Задаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);

    //Отвязываем текстуру, чтобы она не учавствовала в дальнейшей работе
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}