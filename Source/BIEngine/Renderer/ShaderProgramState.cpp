#include "ShaderProgramState.h"

namespace BIEngine
{

    ShaderProgramState::ShaderProgramState(const std::shared_ptr<ShaderProgram>& pShader)
        : m_pShaderProgram(pShader)
    {

    }


    void ShaderProgramState::Use()
    {
        m_pShaderProgram->Use();

        for (const auto& uniform : m_uniformBools) {
            m_pShaderProgram->SetBool(uniform.first, uniform.second);
        }

        for (const auto& uniform : m_uniformInts) {
            m_pShaderProgram->SetInteger(uniform.first, uniform.second);
        }

        for (const auto& uniform : m_uniformFloats) {
            m_pShaderProgram->SetFloat(uniform.first, uniform.second);
        }

        for (const auto& uniform : m_uniformVectors) {
            m_pShaderProgram->SetVector3f(uniform.first, uniform.second);
        }

        for (const auto& uniform : m_uniformMatricies) {
            m_pShaderProgram->SetMatrix4(uniform.first, uniform.second);
        }
    }


    void ShaderProgramState::SetBool(const std::string& name, bool value)
    {
        m_uniformBools[name] = value;
    }


    void ShaderProgramState::SetInteger(const std::string& name, int value)
    {
        m_uniformInts[name] = value;
    }


    void ShaderProgramState::SetFloat(const std::string& name, float value)
    {
        m_uniformFloats[name] = value;
    }


    void ShaderProgramState::SetVector3f(const std::string& name, float x, float y, float z)
    {
        m_uniformVectors[name] = glm::vec3(x, y, z);
    }


    void ShaderProgramState::SetVector3f(const std::string& name, const glm::vec3& vec)
    {
        m_uniformVectors[name] = vec;
    }


    void ShaderProgramState::SetMatrix4(const std::string& name, const glm::mat4& mat)
    {
        m_uniformMatricies[name] = mat;
    }

}