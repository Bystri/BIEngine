#include "Material.h"

namespace BIEngine {

Material::Material(std::shared_ptr<ShaderProgram> pShader)
   : m_color(WHITE), m_isDoubleSided(false),
     m_diffuseMap(nullptr),
     m_renderState(), m_pShaderProgram(pShader)
{
}

ShaderProgramState Material::ConstructShaderProgramState() const
{
   ShaderProgramState shaderProgramState(m_pShaderProgram);
   shaderProgramState.SetVector3f("material.color", m_color);
   shaderProgramState.SetInteger("material.diffuse", 0);
   shaderProgramState.AddTexture(0, m_diffuseMap);

   return shaderProgramState;
}


} // namespace BIEngine
