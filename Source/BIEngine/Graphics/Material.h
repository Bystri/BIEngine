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

   void SetDoubleSided(bool enable) { m_isDoubleSided = enable; }

   bool IsDoubleSided() const { return m_isDoubleSided; }

   void SetBool(const std::string& name, bool value);
   void SetInteger(const std::string& name, int value);
   void SetFloat(const std::string& name, float value);
   void SetColorRgb(const std::string& name, const ColorRgb& color);
   void SetColorRgba(const std::string& name, const ColorRgba& color);

   void AddTexture(const std::string& name, int slotId, std::shared_ptr<Texture> pTexture);

   RenderState& GetRenderState() { return m_renderState; }

   std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pShaderProgram; }

   virtual ShaderProgramState ConstructShaderProgramState() const;


private:
   bool m_isDoubleSided;

   std::shared_ptr<Texture2D> m_diffuseMap;

   RenderState m_renderState;
   std::shared_ptr<ShaderProgram> m_pShaderProgram;

   std::unordered_map<std::string, bool> m_uniformBools;
   std::unordered_map<std::string, int> m_uniformInts;
   std::unordered_map<std::string, float> m_uniformFloats;
   std::unordered_map<std::string, ColorRgb> m_uniformColorsRgb;
   std::unordered_map<std::string, ColorRgba> m_uniformColorsRgba;

   std::vector<std::pair<int, std::shared_ptr<Texture>>> m_textures;
};

} // namespace BIEngine
