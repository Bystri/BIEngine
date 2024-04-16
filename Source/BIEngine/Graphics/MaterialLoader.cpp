#include "MaterialLoader.h"

#include <tinyxml2.h>

#include "../Renderer/TextureLoader.h"
#include "../Renderer/ShadersLoader.h"

namespace BIEngine {

static void materialLoaderLoadColorsRgb(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pColorsRgbElement = pRoot->FirstChildElement("ColorsRgb");
   if (!pColorsRgbElement) {
      return;
   }

   for (tinyxml2::XMLElement* pColorElement = pColorsRgbElement->FirstChildElement(); pColorElement; pColorElement = pColorElement->NextSiblingElement()) {
      const char* paramName;
      ColorRgb color;

      pColorElement->QueryStringAttribute("paramName", &paramName);
      pColorElement->QueryFloatAttribute("r", &color.r);
      pColorElement->QueryFloatAttribute("g", &color.g);
      pColorElement->QueryFloatAttribute("b", &color.b);

      pMaterial->SetColorRgb(paramName, color);
   }
}

static void materialLoaderLoadColorsRgba(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pColorsRgbaElement = pRoot->FirstChildElement("ColorsRgba");
   if (!pColorsRgbaElement) {
      return;
   }

   for (tinyxml2::XMLElement* pColorElement = pColorsRgbaElement->FirstChildElement(); pColorElement; pColorElement = pColorElement->NextSiblingElement()) {
      const char* paramName;
      ColorRgba color;

      pColorElement->QueryStringAttribute("paramName", &paramName);
      pColorElement->QueryFloatAttribute("r", &color.r);
      pColorElement->QueryFloatAttribute("g", &color.g);
      pColorElement->QueryFloatAttribute("b", &color.b);
      pColorElement->QueryFloatAttribute("a", &color.a);

      pMaterial->SetColorRgba(paramName, color);
   }
}

static void materialLoaderLoadIntegers(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pIntegersElement = pRoot->FirstChildElement("Integers");
   if (!pIntegersElement) {
      return;
   }

   for (tinyxml2::XMLElement* pIntegerElement = pIntegersElement->FirstChildElement(); pIntegerElement; pIntegerElement = pIntegerElement->NextSiblingElement()) {
      const char* paramName;
      int val;

      pIntegerElement->QueryStringAttribute("paramName", &paramName);
      pIntegerElement->QueryIntAttribute("i", &val);

      pMaterial->SetInteger(paramName, val);
   }
}

static void materialLoaderLoadFloats(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pFloatsElement = pRoot->FirstChildElement("Floats");
   if (!pFloatsElement) {
      return;
   }

   for (tinyxml2::XMLElement* pFloatElement = pFloatsElement->FirstChildElement(); pFloatElement; pFloatElement = pFloatElement->NextSiblingElement()) {
      const char* paramName;
      float val;

      pFloatElement->QueryStringAttribute("paramName", &paramName);
      pFloatElement->QueryFloatAttribute("f", &val);

      pMaterial->SetFloat(paramName, val);
   }
}

static void materialLoaderLoadBools(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pBoolsEelemnt = pRoot->FirstChildElement("Bools");
   if (!pBoolsEelemnt) {
      return;
   }

   for (tinyxml2::XMLElement* pBoolElement = pBoolsEelemnt->FirstChildElement(); pBoolElement; pBoolElement = pBoolElement->NextSiblingElement()) {
      const char* paramName;
      bool val;

      pBoolElement->QueryStringAttribute("paramName", &paramName);
      pBoolElement->QueryBoolAttribute("b", &val);

      pMaterial->SetBool(paramName, val);
   }
}

static void materialLoaderLoadTextures(tinyxml2::XMLElement* pRoot, std::shared_ptr<Material> pMaterial)
{
   tinyxml2::XMLElement* pTexuresElement = pRoot->FirstChildElement("Textures");
   if (!pTexuresElement) {
      return;
   }

   for (tinyxml2::XMLElement* pTextureElement = pTexuresElement->FirstChildElement(); pTextureElement; pTextureElement = pTextureElement->NextSiblingElement()) {
      const char* paramName;
      int slot;
      const char* texturePath;

      pTextureElement->QueryStringAttribute("paramName", &paramName);
      pTextureElement->QueryIntAttribute("slot", &slot);
      pTextureElement->QueryStringAttribute("path", &texturePath);

      auto textureData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(texturePath)->GetExtra());

      if (textureData == nullptr) {
         return;
      }

      pMaterial->AddTexture(paramName, slot, textureData->GetTexture());
   }
}

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

   std::shared_ptr<Material> pMaterial = std::make_shared<Material>(shaderProgramData->GetShaderProgram());

   tinyxml2::XMLElement* pParamsElement = pRoot->FirstChildElement("Params");
   if (!pParamsElement) {
      return false;
   }

   bool isDoubleSided = false;
   pParamsElement->QueryBoolAttribute("isDoubleSided", &isDoubleSided);
   pMaterial->SetDoubleSided(isDoubleSided);

   materialLoaderLoadColorsRgb(pRoot, pMaterial);
   materialLoaderLoadColorsRgba(pRoot, pMaterial);
   materialLoaderLoadIntegers(pRoot, pMaterial);
   materialLoaderLoadFloats(pRoot, pMaterial);
   materialLoaderLoadBools(pRoot, pMaterial);
   materialLoaderLoadTextures(pRoot, pMaterial);

   std::shared_ptr<MaterialData> pExtra = std::make_shared<MaterialData>();
   pExtra->m_pMaterial = pMaterial;
   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
