#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
    Texture2D();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    //�������� �������� ��������
    void Bind() const;

    //������ ����������� � ��������
    unsigned int GetWidth() const { return m_width; }
    //������ ����������� � ��������
    unsigned int GetHeight() const { return m_height; }

    //������ ������ ������ ��������
    void SetInternalFormat(unsigned int format) { m_internalFormat = format; }
    //������ ������ ������ ��������
    unsigned int GetInternalFormat() const { return m_internalFormat; }
    //������ ������ ������ �����������
    void SetImageFormat(unsigned int format) { m_imageFormat = format; }
    //������ ������ ������ �����������
    unsigned int GetImageFormat() const { return m_imageFormat; }

private:
    unsigned int m_id;

    //������� ������������ ����������� � ��������
    unsigned int m_width, m_height; 

    //������ ��������
    unsigned int m_internalFormat;
    //������ �����������
    unsigned int m_imageFormat;

  
    unsigned int m_wrapS;
    unsigned int m_wrapT;
    unsigned int m_filterMin;
    unsigned int m_filterMax;
};