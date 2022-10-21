#include "shader.h"

#include <iostream>

#include "../Utilities/Logger.h"
#include "ShadersLoader.h"

namespace BIEngine
{

    Shader::~Shader()
    {
        glDeleteProgram(m_id);
    }

    Shader::Shader(Shader&& orig)
    {
        m_id = orig.m_id;

        //Нулевой id является аналогом нулевому указателю, то есть шейдерной программы с таким id не существует
        orig.m_id = 0;
    }

    Shader& Shader::operator=(Shader&& orig)
    {
        if (this == &orig)
            return *this;

        glDeleteProgram(m_id);
        m_id = orig.m_id;

        orig.m_id = 0;

        return *this;
    }

    //Делает шейдер активным
    Shader& Shader::Use()
    {
        glUseProgram(m_id);
        return *this;
    }

    void Shader::Compile(unsigned int sVertex, unsigned int sFragment)
    {
        m_id = glCreateProgram();
        glAttachShader(m_id, sVertex);
        glAttachShader(m_id, sFragment);
        glLinkProgram(m_id);
        CheckCompileErrors(m_id, "PROGRAM");
    }

    void Shader::SetFloat(const char* name, float value, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform1f(glGetUniformLocation(m_id, name), value);
    }

    void Shader::SetInteger(const char* name, int value, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform1i(glGetUniformLocation(m_id, name), value);
    }

    void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform2f(glGetUniformLocation(m_id, name), x, y);
    }

    void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform2f(glGetUniformLocation(m_id, name), value.x, value.y);
    }

    void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform3f(glGetUniformLocation(m_id, name), x, y, z);
    }

    void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform3f(glGetUniformLocation(m_id, name), value.x, value.y, value.z);
    }

    void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform4f(glGetUniformLocation(m_id, name), x, y, z, w);
    }

    void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniform4f(glGetUniformLocation(m_id, name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
    {
        if (useShader)
            this->Use();
        glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, false, glm::value_ptr(matrix));
    }


    //Проверяет и, в случае обнаружуения, выводит сообщения об ошибках во время компиляции шейдера
    void Shader::CheckCompileErrors(unsigned int object, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(object, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(object, 1024, NULL, infoLog);
                Logger::WriteLog(Logger::LogType::ERROR, "Shader compile - time error : Type: " + type + "\n" + infoLog + "\n");
            }
        }
        else
        {
            glGetProgramiv(object, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(object, 1024, NULL, infoLog);
                Logger::WriteLog(Logger::LogType::ERROR, "Shader link-time error: Type: " + type + "\n" + infoLog + "\n");
            }
        }
    }

}