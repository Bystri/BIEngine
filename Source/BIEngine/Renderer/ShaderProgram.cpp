#include "ShaderProgram.h"

#include <iostream>

#include "../Utilities/Logger.h"
#include "ShadersLoader.h"

namespace BIEngine {

ShaderProgram::~ShaderProgram()
{
   if (m_id != 0) {
      glDeleteProgram(m_id);
   }
}

ShaderProgram::ShaderProgram(ShaderProgram&& orig)
{
   m_id = orig.m_id;

   // Нулевой id является аналогом нулевому указателю, то есть шейдерной программы с таким id не существует
   orig.m_id = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& orig)
{
   if (this == &orig)
      return *this;

   if (m_id != 0) {
      glDeleteProgram(m_id);
   }
   m_id = orig.m_id;

   orig.m_id = 0;

   return *this;
}

// Делает шейдер активным
ShaderProgram& ShaderProgram::Use()
{
   static int currentId = -1;

   if (currentId == m_id) {
      return *this;
   }

   currentId = m_id;

   glUseProgram(m_id);
   return *this;
}

void ShaderProgram::Compile(unsigned int sVertex, unsigned int sFragment)
{
   if (m_id != 0) {
      Logger::WriteLog(Logger::LogType::WARNING, "Trying to compile already compiled shader program with index " + std::to_string(m_id));
      return;
   }

   m_id = glCreateProgram();
   glAttachShader(m_id, sVertex);
   glAttachShader(m_id, sFragment);
   glLinkProgram(m_id);
   CheckCompileErrors(m_id, "PROGRAM");
}

void ShaderProgram::Compile(unsigned int sVertex, unsigned int sFragment, unsigned int sGeomtry)
{
   if (m_id != 0) {
      Logger::WriteLog(Logger::LogType::WARNING, "Trying to compile already compiled shader program with index " + std::to_string(m_id));
      return;
   }

   m_id = glCreateProgram();
   glAttachShader(m_id, sVertex);
   glAttachShader(m_id, sGeomtry);
   glAttachShader(m_id, sFragment);
   glLinkProgram(m_id);
   CheckCompileErrors(m_id, "PROGRAM");
}

void ShaderProgram::SetBool(const std::string& name, bool value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void ShaderProgram::SetFloat(const std::string& name, float value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::SetInteger(const std::string& name, int value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::SetVector2f(const std::string& name, float x, float y, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void ShaderProgram::SetVector2f(const std::string& name, const glm::vec2& value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform2f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y);
}

void ShaderProgram::SetVector3f(const std::string& name, float x, float y, float z, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void ShaderProgram::SetVector3f(const std::string& name, const glm::vec3& value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform3f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
}

void ShaderProgram::SetVector4f(const std::string& name, float x, float y, float z, float w, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void ShaderProgram::SetVector4f(const std::string& name, const glm::vec4& value, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform4f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z, value.w);
}

void ShaderProgram::SetMatrix4(const std::string& name, const glm::mat4& matrix, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(matrix));
}

void ShaderProgram::SetColorRgb(const std::string& name, const ColorRgb& color, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform3f(glGetUniformLocation(m_id, name.c_str()), color.r, color.g, color.b);
}

void ShaderProgram::SetColorRgba(const std::string& name, const ColorRgba& color, bool useShader)
{
   if (useShader) {
      this->Use();
   }

   glUniform4f(glGetUniformLocation(m_id, name.c_str()), color.r, color.g, color.b, color.a);
}

// Проверяет и, в случае обнаружуения, выводит сообщения об ошибках во время компиляции шейдера
void ShaderProgram::CheckCompileErrors(unsigned int object, std::string type)
{
   int success;
   char infoLog[1024];
   if (type != "PROGRAM") {
      glGetShaderiv(object, GL_COMPILE_STATUS, &success);
      if (!success) {
         glGetShaderInfoLog(object, 1024, NULL, infoLog);
         Logger::WriteLog(Logger::LogType::ERROR, "Shader compile - time error : Type: " + type + "\n" + infoLog + "\n");
      }
   } else {
      glGetProgramiv(object, GL_LINK_STATUS, &success);
      if (!success) {
         glGetProgramInfoLog(object, 1024, NULL, infoLog);
         Logger::WriteLog(Logger::LogType::ERROR, "Shader link-time error: Type: " + type + "\n" + infoLog + "\n");
      }
   }
}

} // namespace BIEngine
