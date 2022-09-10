#include "ShadersLoaderOpenGL.h"

#include <iostream>

#include "../../Utilities/Logger.h"

namespace BIEngine
{

    OpenglShaderData::OpenglShaderData()
        : m_shaderIndex(0)
    {
    }


    void ShaderResourceLoader::CheckCompileErrors(unsigned int object, std::string type)
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


    bool VertexShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
    {
        std::shared_ptr<OpenglShaderData> pExtra = std::make_shared<OpenglShaderData>();

        unsigned int sVertex;

        //Загружаем код шейдера
        std::string shaderCode(rawBuffer, rawSize);
        const char* code = shaderCode.c_str();

        //Компилируем шейдер
        sVertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(sVertex, 1, &code, NULL);
        glCompileShader(sVertex);
        CheckCompileErrors(sVertex, "VERTEX");

        pExtra->m_shaderIndex = sVertex;
        pHandle->SetExtra(pExtra);

        return true;
    }


    bool FragmentShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
    {
        std::shared_ptr<OpenglShaderData> pExtra = std::make_shared<OpenglShaderData>();

        unsigned int sFragment;
        
        //Загружаем код шейдера
        std::string shaderCode(rawBuffer, rawSize);
        const char* code = shaderCode.c_str();

        //Компилируем шейдер
        sFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(sFragment, 1, &code, NULL);
        glCompileShader(sFragment);
        CheckCompileErrors(sFragment, "FRAGMENT");

        pExtra->m_shaderIndex = sFragment;
        pHandle->SetExtra(pExtra);

        return true;
    }

}