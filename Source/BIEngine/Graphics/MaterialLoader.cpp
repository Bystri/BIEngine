#include "MaterialLoader.h"

#include <tinyxml2.h>

#include "../Renderer/TextureLoader.h"
#include "../Renderer/ShadersLoader.h"

namespace BIEngine {

bool MaterialResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   tinyxml2::XMLDocument xmlDoc;
   tinyxml2::XMLError error = xmlDoc.Parse(rawBuffer, rawSize);

   if (error != tinyxml2::XMLError::XML_SUCCESS) {
      return false;
   }

   tinyxml2::XMLElement* pRoot = xmlDoc.RootElement();
   if (!pRoot) {
      return false;
   }

   tinyxml2::XMLElement* pShaderProgramElement = pRoot->FirstChildElement("ShaderProgram");
   if (!pShaderProgramElement) {
      return false;
   }

   const char* shaderProgramPath;
   pShaderProgramElement->QueryStringAttribute("path", &shaderProgramPath);

   auto shaderProgramData = std::dynamic_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(shaderProgramPath)->GetExtra());

   if (shaderProgramData == nullptr) {
      return false;
   }

   std::shared_ptr<LightReflectiveMaterial> pMaterial = std::make_shared<LightReflectiveMaterial>(shaderProgramData->GetShaderProgram());

   tinyxml2::XMLElement* pDiffuseColorElement = pRoot->FirstChildElement("DiffuseColor");
   if (pDiffuseColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pDiffuseColorElement->QueryFloatAttribute("r", &r);
      pDiffuseColorElement->QueryFloatAttribute("g", &g);
      pDiffuseColorElement->QueryFloatAttribute("b", &b);
      const ColorRgb diffuseColor = ColorRgb(r, g, b);
      pMaterial->SetColor(diffuseColor);
   }

   tinyxml2::XMLElement* pDiffuseMapElement = pRoot->FirstChildElement("DiffuseMap");
   if (pDiffuseMapElement) {
      const char* path;
      pDiffuseMapElement->QueryStringAttribute("path", &path);

      auto diffuseMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(path)->GetExtra());

      if (diffuseMapData == nullptr) {
         return false;
      }

      pMaterial->SetDiffuseMap(diffuseMapData->GetTexture());
   } else {
      auto diffuseMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/diffuse_default.bitf")->GetExtra());

      if (diffuseMapData == nullptr) {
         return false;
      }

      pMaterial->SetDiffuseMap(diffuseMapData->GetTexture());
   }

   tinyxml2::XMLElement* pSpecularMapElement = pRoot->FirstChildElement("SpecularMap");
   if (pSpecularMapElement) {
      const char* path;
      pSpecularMapElement->QueryStringAttribute("path", &path);

      auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(path)->GetExtra());

      if (specularMapData == nullptr) {
         return false;
      }

      pMaterial->SetSpecularMap(specularMapData->GetTexture());
   } else {
      auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

      if (specularMapData == nullptr) {
         return false;
      }

      pMaterial->SetSpecularMap(specularMapData->GetTexture());
   }

   tinyxml2::XMLElement* pNormalMapElement = pRoot->FirstChildElement("NormalMap");
   if (pNormalMapElement) {
      const char* path;
      pNormalMapElement->QueryStringAttribute("path", &path);

      auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(path)->GetExtra());

      if (normalMapData == nullptr) {
         return false;
      }

      pMaterial->SetNormalMap(normalMapData->GetTexture());
   } else {
      auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

      if (normalMapData == nullptr) {
         return false;
      }

      pMaterial->SetNormalMap(normalMapData->GetTexture());
   }

   tinyxml2::XMLElement* pDisplacementMapElement = pRoot->FirstChildElement("DisplacementMap");
   if (pDisplacementMapElement) {
      const char* path;
      pDisplacementMapElement->QueryStringAttribute("path", &path);

      auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(path)->GetExtra());

      if (displacementMapData == nullptr) {
         return false;
      }

      pMaterial->SetNormalMap(displacementMapData->GetTexture());
   } else {
      auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

      if (displacementMapData == nullptr) {
         return false;
      }

      pMaterial->SetNormalMap(displacementMapData->GetTexture());
   }

   std::shared_ptr<MaterialData> pExtra = std::make_shared<MaterialData>();
   pExtra->m_pMaterial = pMaterial;
   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
