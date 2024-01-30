#pragma once

#include <glm/glm.hpp>

#include "../Renderer/Color.h"
#include "../Renderer/Texture.h"
#include "../Renderer/RenderState.h"
#include "../Renderer/ShaderProgramState.h"

namespace BIEngine {

// Материал отвечает за прозрачность и цвет спрайта
class Material {
public:
   explicit Material(std::shared_ptr<ShaderProgram> pShader);

   void SetColor(const ColorRgba& color) { m_color = color; }

   const ColorRgba& GetColor() { return m_color; }

   void SetDoubleSided(bool enable) { m_isDoubleSided = enable; }

   bool IsDoubleSided() const { return m_isDoubleSided; }

   void SetDiffuseMap(std::shared_ptr<Texture2D> diffuseMap) { m_diffuseMap = diffuseMap; }

   std::shared_ptr<Texture2D> GetDiffuseMap() const { return m_diffuseMap; }

   bool HasAlpha() const { return std::abs(m_color.a - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon(); }

   RenderState& GetRenderState() { return m_renderState; }

   std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pShaderProgram; }

   virtual ShaderProgramState ConstructShaderProgramState() const;


private:
   ColorRgba m_color;

   bool m_isDoubleSided;

   std::shared_ptr<Texture2D> m_diffuseMap;

   RenderState m_renderState;
   std::shared_ptr<ShaderProgram> m_pShaderProgram;
};

} // namespace BIEngine
