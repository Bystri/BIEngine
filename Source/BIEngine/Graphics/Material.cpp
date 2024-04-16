#include "Material.h"

namespace BIEngine {

Material::Material(std::shared_ptr<ShaderProgram> pShader)
   : m_isDoubleSided(false),
     m_uniformBools(), m_uniformInts(), m_uniformFloats(), m_uniformColorsRgb(), m_textures(),
     m_renderState(), m_pShaderProgram(pShader)
{
}

void Material::SetBool(const std::string& name, bool value)
{
   m_uniformBools[name] = value;
}

void Material::SetInteger(const std::string& name, int value)
{
   m_uniformInts[name] = value;
}

void Material::SetFloat(const std::string& name, float value)
{
   m_uniformFloats[name] = value;
}

void Material::SetColorRgb(const std::string& name, const ColorRgb& color)
{
   m_uniformColorsRgb[name] = color;
}

void Material::SetColorRgba(const std::string& name, const ColorRgba& color)
{
   m_uniformColorsRgba[name] = color;
}

void Material::AddTexture(const std::string& name, int slotId, std::shared_ptr<Texture> pTexture)
{
   m_uniformInts[name] = slotId;
   m_textures.push_back({slotId, pTexture});
}

ShaderProgramState Material::ConstructShaderProgramState() const
{
   ShaderProgramState shaderProgramState(m_pShaderProgram);

   for (const auto& uniform : m_uniformBools) {
      shaderProgramState.SetBool(uniform.first, uniform.second);
   }

   for (const auto& uniform : m_uniformInts) {
      shaderProgramState.SetInteger(uniform.first, uniform.second);
   }

   for (const auto& uniform : m_uniformFloats) {
      shaderProgramState.SetFloat(uniform.first, uniform.second);
   }

   for (const auto& uniform : m_uniformColorsRgb) {
      shaderProgramState.SetColorRgb(uniform.first, uniform.second);
   }

   for (const auto& uniform : m_uniformColorsRgba) {
      shaderProgramState.SetColorRgba(uniform.first, uniform.second);
   }

   for (const auto& texture : m_textures) {
      shaderProgramState.AddTexture(texture.first, texture.second);
   }

   return shaderProgramState;
}


} // namespace BIEngine
