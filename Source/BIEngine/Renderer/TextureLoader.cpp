#include "TextureLoader.h"

#include <tinyxml2.h>

#include "../Renderer/ImageLoader.h"

namespace BIEngine {

Texture2D::TextureWrap textureLoaderGetTextureWrapFromParameter(const char* parameter)
{
   if (std::strcmp(parameter, "Repeat")) {
      return Texture2D::TextureWrap::REPEAT;
   } else if (std::strcmp(parameter, "ClampToEdge")) {
      return Texture2D::TextureWrap::CLAMP_TO_EDGE;
   } else if (std::strcmp(parameter, "ClampToBorder")) {
      return Texture2D::TextureWrap::CLAMP_TO_BORDER;
   }

   assert(false && "There is a type in Wrap parameter of Texture");
   return Texture2D::TextureWrap::REPEAT;
}

Texture2D::TextureFunction textureLoaderGetTextureFunctionFromParameter(const char* parameter)
{
   if (std::strcmp(parameter, "Linear")) {
      return Texture2D::TextureFunction::LINEAR;
   } else if (std::strcmp(parameter, "Nearest")) {
      return Texture2D::TextureFunction::NEAREST;
   }

   assert(false && "There is a type in Function parameter of Texture");
   return Texture2D::TextureFunction::NEAREST;
}

Texture2D::CreationParams textureLoaderGetCreationParams(tinyxml2::XMLElement* pParamsElement)
{
   Texture2D::CreationParams params;
   if (!pParamsElement) {
      return params;
   }

   const char* paramsText;
   if (pParamsElement->QueryStringAttribute("wrapS", &paramsText) == tinyxml2::XMLError::XML_SUCCESS) {
      params.WrapS = textureLoaderGetTextureWrapFromParameter(paramsText);
   }

   if (pParamsElement->QueryStringAttribute("wrapT", &paramsText) == tinyxml2::XMLError::XML_SUCCESS) {
      params.WrapT = textureLoaderGetTextureWrapFromParameter(paramsText);
   }

   if (pParamsElement->QueryStringAttribute("filterMin", &paramsText) == tinyxml2::XMLError::XML_SUCCESS) {
      params.FilterMin = textureLoaderGetTextureFunctionFromParameter(paramsText);
   }

   if (pParamsElement->QueryStringAttribute("filterMax", &paramsText) == tinyxml2::XMLError::XML_SUCCESS) {
      params.FilterMax = textureLoaderGetTextureFunctionFromParameter(paramsText);
   }

   return params;
}

bool TextureResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
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

   tinyxml2::XMLElement* pImgDataElement = pRoot->FirstChildElement("ImgData");
   if (!pImgDataElement) {
      return false;
   }

   const char* texturePath;
   if (pImgDataElement->QueryStringAttribute("path", &texturePath) != tinyxml2::XMLError::XML_SUCCESS) {
      return false;
   }

   auto imgData = std::dynamic_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(texturePath)->GetExtra());

   if (!imgData) {
      return false;
   }

   tinyxml2::XMLElement* pParams = pRoot->FirstChildElement("CreationParams");
   Texture2D::CreationParams params = textureLoaderGetCreationParams(pParams);

   Texture2D::SizedFormat sizedFormat = imgData->GetChannelsNum() > 3 ? Texture2D::SizedFormat::RGBA : Texture2D::SizedFormat::RGB;
   Texture2D::Format texureFormat = imgData->GetChannelsNum() > 3 ? Texture2D::Format::RGBA : Texture2D::Format::RGB;

   std::shared_ptr<TextureData> pExtra = std::make_shared<TextureData>();
   pExtra->m_pTexture = Texture2D::Create(imgData->GetWidth(), imgData->GetHeight(), sizedFormat, texureFormat, imgData->GetData());

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
