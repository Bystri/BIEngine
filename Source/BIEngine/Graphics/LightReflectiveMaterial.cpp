#include "LightReflectiveMaterial.h"

namespace BIEngine {

LightReflectiveMaterial::LightReflectiveMaterial(std::shared_ptr<ShaderProgram> pShader)
   : Material(pShader), m_specularMap(nullptr), m_shininess(64.0f)
{
}

ShaderProgramState LightReflectiveMaterial::ConstructShaderProgramState() const
{
   ShaderProgramState shaderProgramState = Material::ConstructShaderProgramState();
   shaderProgramState.SetFloat("material.shininess", m_shininess);
   shaderProgramState.SetInteger("material.specular", 1);
   shaderProgramState.SetInteger("material.normal", 2);
   shaderProgramState.AddTexture(1, m_specularMap);
   shaderProgramState.AddTexture(2, m_normalMap);

   return shaderProgramState;
}


} // namespace BIEngine
