#include "ShadersLoader.h"

#include <tinyxml2.h>

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

static void shaderLoaderCheckCompileErrors(unsigned int object, const std::string& type, const std::string& name)
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

bool GeometryShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<ShaderData> pExtra = std::make_shared<ShaderData>();

   unsigned int sGeometry;

   // Загружаем код шейдера
   const std::string rawShaderCode(rawBuffer, rawSize);
   const std::string codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.c_str();

   // Компилируем шейдер
   sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
   glShaderSource(sGeometry, 1, &cstrCodeToCompile, NULL);
   glCompileShader(sGeometry);
   shaderLoaderCheckCompileErrors(sGeometry, "GEOMETRY", pHandle->GetName());

   pExtra->m_shaderIndex = sGeometry;
   pHandle->SetExtra(pExtra);

   return true;
}

enum class ShaderType {
   VERTEX_SHADER,
   FRAGMENT_SHADER,
   GEOMETRY_SHADER
};

bool ShaderProgramResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   tinyxml2::XMLDocument xmlDoc;
   tinyxml2::XMLError error = xmlDoc.Parse(rawBuffer, rawSize);

   int vertexShaderIdx = -1;
   int fragmentShaderIdx = -1;
   int geometryShaderIdx = -1;

   if (error != tinyxml2::XMLError::XML_SUCCESS) {
      return false;
   }

   tinyxml2::XMLElement* pRoot = xmlDoc.RootElement();
   if (!pRoot) {
      return false;
   }

   tinyxml2::XMLElement* pVertexShader = pRoot->FirstChildElement("Vertex");
   if (!pVertexShader) {
      return false;
   }

   {
      const char* shaderPath;
      pVertexShader->QueryStringAttribute("path", &shaderPath);

      auto pShaderData = std::dynamic_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (pShaderData == nullptr) {
         return false;
      }

      vertexShaderIdx = pShaderData->GetShaderIndex();
   }

   tinyxml2::XMLElement* pFragmentShader = pRoot->FirstChildElement("Fragment");
   if (!pFragmentShader) {
      return false;
   }

   {
      const char* shaderPath;
      pFragmentShader->QueryStringAttribute("path", &shaderPath);

      auto pShaderData = std::dynamic_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (pShaderData == nullptr) {
         return false;
      }

      fragmentShaderIdx = pShaderData->GetShaderIndex();
   }

   tinyxml2::XMLElement* pGeometryShader = pRoot->FirstChildElement("Geometry");
   if (pGeometryShader) {
      const char* shaderPath;
      pGeometryShader->QueryStringAttribute("path", &shaderPath);

      auto pShaderData = std::dynamic_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (pShaderData == nullptr) {
         return false;
      }

      geometryShaderIdx = pShaderData->GetShaderIndex();
   }

   if (vertexShaderIdx == -1 || fragmentShaderIdx == -1) {
      Logger::WriteLog(Logger::LogType::ERROR, "Shader program " + pHandle->GetName() + " must have vertex and geometry shaders!");
      return false;
   }


   std::shared_ptr<ShaderProgramData> pExtra = std::make_shared<ShaderProgramData>();
   pExtra->m_pShaderProgram = std::make_shared<ShaderProgram>();

   if (geometryShaderIdx == -1) {
      pExtra->m_pShaderProgram->Compile(vertexShaderIdx, fragmentShaderIdx);
   } else {
      pExtra->m_pShaderProgram->Compile(vertexShaderIdx, fragmentShaderIdx, geometryShaderIdx);
   }

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
