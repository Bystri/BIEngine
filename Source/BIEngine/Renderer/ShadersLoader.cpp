#include "ShadersLoader.h"

#include <sstream>
#include <string>
#include <iostream>

#include "../../Utilities/Logger.h"

namespace BIEngine {

static std::string shaderLoaderReadShaderCode(std::istringstream& shaderCode, const std::string& fileName)
{
   std::string source, line;
   while (std::getline(shaderCode, line)) {
      if (line.substr(0, 8) == "#include") {
         std::string includePath = line.substr(9);

         const size_t formatIndex = includePath.find(".glsl");
         if (formatIndex == std::string::npos) {
            Logger::WriteLog(Logger::LogType::ERROR, "Include file must be *.glsl in shader " + fileName);
            return std::string();
         }

         includePath = includePath.substr(0, formatIndex + 5);
         const std::shared_ptr<UtilityShaderData> pShaderData = std::static_pointer_cast<UtilityShaderData>(ResCache::Get()->GetHandle(includePath)->GetExtra());

         if (!pShaderData) {
            Logger::WriteLog(Logger::LogType::ERROR, "Failed to open include file " + includePath + " in shader " + fileName);
            return std::string();
         }

         source += pShaderData->GetUtilityShaderSource();
      } else {
         source += line + "\n";
      }
   }
   return source;
}

bool UtilityShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<UtilityShaderData> pExtra = std::make_shared<UtilityShaderData>();

   const std::string shaderCode(rawBuffer, rawSize);
   pExtra->m_utilityShaderSource = shaderLoaderReadShaderCode(std::istringstream(shaderCode), pHandle->GetName());
   pHandle->SetExtra(pExtra);

   return true;
}

ShaderData::ShaderData()
   : m_shaderIndex(0)
{
}

static void shaderLoaderCheckCompileErrors(unsigned int object, std::string type, const std::string name)
{
   constexpr unsigned int INFO_LOG_BUFFER_SIZE = 1024;

   int success;
   char infoLog[INFO_LOG_BUFFER_SIZE];

   glGetShaderiv(object, GL_COMPILE_STATUS, &success);
   if (!success) {
      glGetShaderInfoLog(object, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
      Logger::WriteLog(Logger::LogType::ERROR, "Shader " + name + " compile - time error : Type: " + type + "\n" + infoLog + "\n");
   }
}

bool VertexShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<ShaderData> pExtra = std::make_shared<ShaderData>();

   unsigned int sVertex;

   // Загружаем код шейдера
   const std::string rawShaderCode(rawBuffer, rawSize);
   const std::string codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.c_str();

   // Компилируем шейдер
   sVertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(sVertex, 1, &cstrCodeToCompile, NULL);
   glCompileShader(sVertex);
   shaderLoaderCheckCompileErrors(sVertex, "VERTEX", pHandle->GetName());

   pExtra->m_shaderIndex = sVertex;
   pHandle->SetExtra(pExtra);

   return true;
}

bool FragmentShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<ShaderData> pExtra = std::make_shared<ShaderData>();

   unsigned int sFragment;

   // Загружаем код шейдера
   const std::string rawShaderCode(rawBuffer, rawSize);
   const std::string codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.c_str();

   // Компилируем шейдер
   sFragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(sFragment, 1, &cstrCodeToCompile, NULL);
   glCompileShader(sFragment);
   shaderLoaderCheckCompileErrors(sFragment, "FRAGMENT", pHandle->GetName());

   pExtra->m_shaderIndex = sFragment;
   pHandle->SetExtra(pExtra);

   return true;
}

enum class ShaderType {
   VERTEX_SHADER,
   FRAGMENT_SHADER
};

static bool shaderLoaderReadShaderSettings(const std::string& shaderSettingsString, ShaderType& shaderType, std::string& fileName)
{
   if (shaderSettingsString[0] != '#') {
      return false;
   }

   const size_t endTypeIdx = shaderSettingsString.find(':');
   if (endTypeIdx == std::string::npos) {
      return false;
   }

   const std::string typeName = shaderSettingsString.substr(1, endTypeIdx - 1);
   if (typeName == "vertex") {
      shaderType = ShaderType::VERTEX_SHADER;
   } else if (typeName == "fragment") {
      shaderType = ShaderType::FRAGMENT_SHADER;
   } else {
      return false;
   }

   const size_t endPathIdx = shaderSettingsString.find(';');
   fileName = shaderSettingsString.substr(endTypeIdx + 1, endPathIdx - endTypeIdx - 1);

   return true;
}

bool ShaderProgramResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   const std::string shaderProgramCode(rawBuffer, rawSize);
   std::istringstream bufferStream = std::istringstream(shaderProgramCode);

   int vertexShaderIdx = -1;
   int fragmentShaderIdx = -1;

   std::string line;
   while (std::getline(bufferStream, line)) {
      ShaderType shaderType;
      std::string shaderPath;

      if (!shaderLoaderReadShaderSettings(line, shaderType, shaderPath)) {
         Logger::WriteLog(Logger::LogType::ERROR, "Incorrect shader settings string int shader program file " + pHandle->GetName());
         return false;
      }

      const std::shared_ptr<ShaderData> pShaderData = std::static_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (!pShaderData) {
         Logger::WriteLog(Logger::LogType::ERROR, "Incorrect shader file path specified in shader program file " + pHandle->GetName());
         return false;
      }

      if (shaderType == ShaderType::VERTEX_SHADER) {
         vertexShaderIdx = pShaderData->GetShaderIndex();
      } else if (shaderType == ShaderType::FRAGMENT_SHADER) {
         fragmentShaderIdx = pShaderData->GetShaderIndex();
      } else {
         Logger::WriteLog(Logger::LogType::ERROR, "Incorrect shader type specified in shader program file " + pHandle->GetName());
         return false;
      }
   }

   std::shared_ptr<ShaderProgramData> pExtra = std::make_shared<ShaderProgramData>();

   pExtra->m_pShaderProgram = std::make_shared<ShaderProgram>();
   pExtra->m_pShaderProgram->Compile(vertexShaderIdx, fragmentShaderIdx);

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
