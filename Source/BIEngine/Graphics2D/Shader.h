#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BIEngine
{

    class Shader
    {
    public:
        Shader() { }

        Shader& Use();

        void Compile(unsigned int sVertex, unsigned int sFragment);


        unsigned int GetId() const { return m_id; }

        //�������, ����������� ������ �������� ���������� ������ �������

        void    SetFloat(const char* name, float value, bool useShader = false);
        void    SetInteger(const char* name, int value, bool useShader = false);
        void    SetVector2f(const char* name, float x, float y, bool useShader = false);
        void    SetVector2f(const char* name, const glm::vec2& value, bool useShader = false);
        void    SetVector3f(const char* name, float x, float y, float z, bool useShader = false);
        void    SetVector3f(const char* name, const glm::vec3& value, bool useShader = false);
        void    SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
        void    SetVector4f(const char* name, const glm::vec4& value, bool useShader = false);
        void    SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false);
    private:
        void    checkCompileErrors(unsigned int object, std::string type);

    private:
        unsigned int m_id;
    };

}