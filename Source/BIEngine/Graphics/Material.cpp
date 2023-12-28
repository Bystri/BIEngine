#include "Material.h"

namespace BIEngine {

Material::Material(std::shared_ptr<ShaderProgram> pShader)
   : m_color(WHITE), m_isDoubleSided(false),
     m_diffuseMap(nullptr),
     m_renderState(), m_pShaderProgram(pShader)
{
}

void Material::SetColor(const Color& color)
{
   m_color = color;
}

bool Material::HasAlpha() const
{
   return std::abs(m_color.a - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon();
}

ShaderProgramState Material::ConstructShaderProgramState() const
{
   ShaderProgramState shaderProgramState(m_pShaderProgram);
   shaderProgramState.SetVector3f("color", m_color);
   shaderProgramState.AddTexture(m_diffuseMap);

   return shaderProgramState;
}


} // namespace BIEngine
