#pragma once

#include "Material.h"

namespace BIEngine {

class LightReflectiveMaterial : public Material {
public:
   explicit LightReflectiveMaterial(std::shared_ptr<ShaderProgram> pShader);

   virtual ~LightReflectiveMaterial() {}

   void SetSpecularMap(std::shared_ptr<Texture2D> specularMap);

   std::shared_ptr<Texture2D> GetSpecularMap() const;

   void SetShininess(float shininess);

   float GetShininess() const;

   virtual ShaderProgramState ConstructShaderProgramState() const override;

private:
   std::shared_ptr<Texture2D> m_specularMap;
   float m_shininess;
};

} // namespace BIEngine
