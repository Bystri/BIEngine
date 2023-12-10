#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "ShaderProgram.h"

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

private:
   const std::shared_ptr<ShaderProgram> m_pShaderProgram;

   std::unordered_map<std::string, bool> m_uniformBools;
   std::unordered_map<std::string, int> m_uniformInts;
   std::unordered_map<std::string, float> m_uniformFloats;
   std::unordered_map<std::string, glm::vec3> m_uniformVectors;
   std::unordered_map<std::string, glm::mat4> m_uniformMatricies;
};

} // namespace BIEngine