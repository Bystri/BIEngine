#pragma once

#include <glm/glm.hpp>

#include "../StdLib/String.h"
#include "../StdLib/HashMap.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/DynamicArray.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Color.h"

namespace BIEngine {

class ShaderProgramState {
public:
   explicit ShaderProgramState(const SharedPtr<ShaderProgram>& pShader);

   void Use();

   void SetBool(const String& name, bool value);
   void SetInteger(const String& name, int value);
   void SetFloat(const String& name, float value);
   void SetVector3f(const String& name, float x, float y, float z);
   void SetVector3f(const String& name, const glm::vec3& vec);
   void SetMatrix4(const String& name, const glm::mat4& mat);
   void SetColorRgb(const String& name, const ColorRgb& color);
   void SetColorRgba(const String& name, const ColorRgba& color);

   void AddTexture(int slotId, SharedPtr<Texture> pTexture);

   std::size_t GetTexturesNum() const { return m_textures.Size(); }

private:
   SharedPtr<ShaderProgram> m_pShaderProgram;

   HashMap<String, bool> m_uniformBools;
   HashMap<String, int> m_uniformInts;
   HashMap<String, float> m_uniformFloats;
   HashMap<String, glm::vec3> m_uniformVectors;
   HashMap<String, glm::mat4> m_uniformMatricies;
   HashMap<String, ColorRgb> m_uniformColorsRgb;
   HashMap<String, ColorRgba> m_uniformColorsRgba;

   DynamicArray<std::pair<int, SharedPtr<Texture>>> m_textures;
};

} // namespace BIEngine
