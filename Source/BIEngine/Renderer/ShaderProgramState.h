#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "../StdLib/HashMap.h"
#include "../StdLib/DynamicArray.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Color.h"

namespace BIEngine {

class ShaderProgramState {
public:
   explicit ShaderProgramState(const std::shared_ptr<ShaderProgram>& pShader);

   void Use();

   void SetBool(const std::string& name, bool value);
   void SetInteger(const std::string& name, int value);
   void SetFloat(const std::string& name, float value);
   void SetVector3f(const std::string& name, float x, float y, float z);
   void SetVector3f(const std::string& name, const glm::vec3& vec);
   void SetMatrix4(const std::string& name, const glm::mat4& mat);
   void SetColorRgb(const std::string& name, const ColorRgb& color);
   void SetColorRgba(const std::string& name, const ColorRgba& color);

   void AddTexture(int slotId, std::shared_ptr<Texture> pTexture);

   std::size_t GetTexturesNum() const { return m_textures.Size(); }

private:
   std::shared_ptr<ShaderProgram> m_pShaderProgram;

   HashMap<std::string, bool> m_uniformBools;
   HashMap<std::string, int> m_uniformInts;
   HashMap<std::string, float> m_uniformFloats;
   HashMap<std::string, glm::vec3> m_uniformVectors;
   HashMap<std::string, glm::mat4> m_uniformMatricies;
   HashMap<std::string, ColorRgb> m_uniformColorsRgb;
   HashMap<std::string, ColorRgba> m_uniformColorsRgba;

   DynamicArray<std::pair<int, std::shared_ptr<Texture>>> m_textures;
};

} // namespace BIEngine
