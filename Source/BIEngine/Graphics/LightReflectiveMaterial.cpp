#include "LightReflectiveMaterial.h"

namespace BIEngine {

LightReflectiveMaterial::LightReflectiveMaterial(std::shared_ptr<ShaderProgram> pShader)
   : Material(pShader), m_specularMap(nullptr), m_shininess(64.0f)
{
}

void LightReflectiveMaterial::SetSpecularMap(std::shared_ptr<Texture2D> specularMap)
{
   m_specularMap = specularMap;
}

std::shared_ptr<Texture2D> LightReflectiveMaterial::GetSpecularMap() const
{
   return m_specularMap;
}

void LightReflectiveMaterial::SetShininess(float shininess)
{
   m_shininess = shininess;
}

float LightReflectiveMaterial::GetShininess() const
{
   return m_shininess;
}

ShaderProgramState LightReflectiveMaterial::ConstructShaderProgramState() const
{
   ShaderProgramState shaderProgramState = Material::ConstructShaderProgramState();
   shaderProgramState.SetFloat("material.shininess", m_shininess);
   shaderProgramState.AddTexture(m_specularMap);

   return shaderProgramState;
}


} // namespace BIEngine
