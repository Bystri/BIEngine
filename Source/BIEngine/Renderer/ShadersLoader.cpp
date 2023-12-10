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

   std::string shaderCode(rawBuffer, rawSize);
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
   std::string shaderCode(rawBuffer, rawSize);
   shaderCode = shaderLoaderReadShaderCode(std::istringstream(shaderCode), pHandle->GetName());
   const char* code = shaderCode.c_str();

   // Компилируем шейдер
   sVertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(sVertex, 1, &code, NULL);
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
   std::string shaderCode(rawBuffer, rawSize);
   shaderCode = shaderLoaderReadShaderCode(std::istringstream(shaderCode), pHandle->GetName());
   const char* code = shaderCode.c_str();

   // Компилируем шейдер
   sFragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(sFragment, 1, &code, NULL);
   glCompileShader(sFragment);
   shaderLoaderCheckCompileErrors(sFragment, "FRAGMENT", pHandle->GetName());

   pExtra->m_shaderIndex = sFragment;
   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine