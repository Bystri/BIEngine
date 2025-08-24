#include "ShadersLoader.h"

#include <tinyxml2.h>

#include <sstream>
#include <iostream>

#include "../StdLib/String.h"
#include "../../Utilities/Logger.h"

namespace BIEngine {

static String shaderLoaderReadShaderCode(std::istringstream& shaderCode, const String& fileName)
{
   String source, line;
   while (Getline(shaderCode, line)) {
      if (line.Substr(0, 8) == "#include") {
         String includePath = line.Substr(9);

         const size_t formatIndex = includePath.Find(".glsl");
         if (formatIndex == String::NPos) {
            Logger::WriteLog(Logger::LogType::ERROR, "Include file must be *.glsl in shader " + fileName);
            return String();
         }

         includePath = includePath.Substr(0, formatIndex + 5);
         const SharedPtr<UtilityShaderData> pShaderData = StaticPointerCast<UtilityShaderData>(ResCache::Get()->GetHandle(includePath)->GetExtra());

         if (!pShaderData) {
            Logger::WriteLog(Logger::LogType::ERROR, "Failed to open include file " + includePath + " in shader " + fileName);
            return String();
         }

         source += pShaderData->GetUtilityShaderSource();
      } else {
         source += line + "\n";
      }
   }
   return source;
}

bool UtilityShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<UtilityShaderData> pExtra = MakeShared<UtilityShaderData>();

   const String shaderCode(rawBuffer, rawSize);
   pExtra->m_utilityShaderSource = shaderLoaderReadShaderCode(std::istringstream(shaderCode.CStr()), pHandle->GetName());
   pHandle->SetExtra(pExtra);

   return true;
}

ShaderData::ShaderData()
   : m_shaderIndex(0)
{
}

static void shaderLoaderCheckCompileErrors(unsigned int object, const String& type, const String& name)
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

bool VertexShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<ShaderData> pExtra = MakeShared<ShaderData>();

   unsigned int sVertex;

   // Загружаем код шейдера
   const String rawShaderCode(rawBuffer, rawSize);
   const String codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode.CStr()), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.CStr();

   // Компилируем шейдер
   sVertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(sVertex, 1, &cstrCodeToCompile, NULL);
   glCompileShader(sVertex);
   shaderLoaderCheckCompileErrors(sVertex, "VERTEX", pHandle->GetName());

   pExtra->m_shaderIndex = sVertex;
   pHandle->SetExtra(pExtra);

   return true;
}

bool FragmentShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<ShaderData> pExtra = MakeShared<ShaderData>();

   unsigned int sFragment;

   // Загружаем код шейдера
   const String rawShaderCode(rawBuffer, rawSize);
   const String codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode.CStr()), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.CStr();

   // Компилируем шейдер
   sFragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(sFragment, 1, &cstrCodeToCompile, NULL);
   glCompileShader(sFragment);
   shaderLoaderCheckCompileErrors(sFragment, "FRAGMENT", pHandle->GetName());

   pExtra->m_shaderIndex = sFragment;
   pHandle->SetExtra(pExtra);

   return true;
}

bool GeometryShaderResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<ShaderData> pExtra = MakeShared<ShaderData>();

   unsigned int sGeometry;

   // Загружаем код шейдера
   const String rawShaderCode(rawBuffer, rawSize);
   const String codeToCompile = shaderLoaderReadShaderCode(std::istringstream(rawShaderCode.CStr()), pHandle->GetName());
   const char* cstrCodeToCompile = codeToCompile.CStr();

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

bool ShaderProgramResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
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

      auto pShaderData = StaticPointerCast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

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

      auto pShaderData = StaticPointerCast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (pShaderData == nullptr) {
         return false;
      }

      fragmentShaderIdx = pShaderData->GetShaderIndex();
   }

   tinyxml2::XMLElement* pGeometryShader = pRoot->FirstChildElement("Geometry");
   if (pGeometryShader) {
      const char* shaderPath;
      pGeometryShader->QueryStringAttribute("path", &shaderPath);

      auto pShaderData = StaticPointerCast<ShaderData>(ResCache::Get()->GetHandle(shaderPath)->GetExtra());

      if (pShaderData == nullptr) {
         return false;
      }

      geometryShaderIdx = pShaderData->GetShaderIndex();
   }

   if (vertexShaderIdx == -1 || fragmentShaderIdx == -1) {
      Logger::WriteLog(Logger::LogType::ERROR, "Shader program " + pHandle->GetName() + " must have vertex and geometry shaders!");
      return false;
   }


   SharedPtr<ShaderProgramData> pExtra = MakeShared<ShaderProgramData>();
   pExtra->m_pShaderProgram = MakeShared<ShaderProgram>();

   if (geometryShaderIdx == -1) {
      pExtra->m_pShaderProgram->Compile(vertexShaderIdx, fragmentShaderIdx);
   } else {
      pExtra->m_pShaderProgram->Compile(vertexShaderIdx, fragmentShaderIdx, geometryShaderIdx);
   }

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
