#pragma once

#include "Material.h"

namespace BIEngine {

class LightReflectiveMaterial : public Material {
public:
   explicit LightReflectiveMaterial(std::shared_ptr<ShaderProgram> pShader);

   virtual ~LightReflectiveMaterial() {}

   void LightReflectiveMaterial::SetSpecularMap(std::shared_ptr<Texture2D> specularMap) { m_specularMap = specularMap; }

   std::shared_ptr<Texture2D> LightReflectiveMaterial::GetSpecularMap() const { return m_specularMap; }

   void LightReflectiveMaterial::SetNormalMap(std::shared_ptr<Texture2D> normalMap) { m_normalMap = normalMap; }

   std::shared_ptr<Texture2D> LightReflectiveMaterial::GetNormalMap() const { return m_normalMap; }

   void LightReflectiveMaterial::SetShininess(float shininess) { m_shininess = shininess; }

   float LightReflectiveMaterial::GetShininess() const { return m_shininess; }

   virtual ShaderProgramState ConstructShaderProgramState() const override;

private:
   std::shared_ptr<Texture2D> m_specularMap;
   std::shared_ptr<Texture2D> m_normalMap;
   float m_shininess;
};

} // namespace BIEngine
