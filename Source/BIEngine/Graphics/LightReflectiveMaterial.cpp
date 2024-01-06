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
   shaderProgramState.SetInteger("material.diffuse", 0);
   shaderProgramState.SetInteger("material.specular", 1);
   shaderProgramState.SetInteger("material.normal", 2);
   shaderProgramState.AddTexture(m_specularMap);
   shaderProgramState.AddTexture(m_normalMap);

   return shaderProgramState;
}


} // namespace BIEngine
