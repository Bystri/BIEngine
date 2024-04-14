#include "AnimationLoader.h"

#include <tinyxml2.h>

namespace BIEngine {

static BoneAnimChannel animationLoaderReadBoneChannel(tinyxml2::XMLElement* const pChannelElement)
{
   const char* boneName;
   pChannelElement->QueryStringAttribute("boneName", &boneName);

   std::vector<BoneAnimChannel::KeyPosition> positions;
   std::vector<BoneAnimChannel::KeyRotation> rotations;
   std::vector<BoneAnimChannel::KeyScale> scales;

   tinyxml2::XMLElement* pPositionsElement = pChannelElement->FirstChildElement("Positions");
   for (tinyxml2::XMLElement* pPosElement = pPositionsElement->FirstChildElement(); pPosElement; pPosElement = pPosElement->NextSiblingElement()) {
      BoneAnimChannel::KeyPosition data;

      pPosElement->QueryFloatAttribute("x", &data.position.x);
      pPosElement->QueryFloatAttribute("y", &data.position.y);
      pPosElement->QueryFloatAttribute("z", &data.position.z);
      pPosElement->QueryFloatAttribute("timestamp", &data.timeStamp);
      positions.push_back(data);
   }

   tinyxml2::XMLElement* pRotationsElement = pChannelElement->FirstChildElement("Rotations");
   for (tinyxml2::XMLElement* pRotElement = pRotationsElement->FirstChildElement(); pRotElement; pRotElement = pRotElement->NextSiblingElement()) {
      BoneAnimChannel::KeyRotation data;

      pRotElement->QueryFloatAttribute("w", &data.orientation.w);
      pRotElement->QueryFloatAttribute("x", &data.orientation.x);
      pRotElement->QueryFloatAttribute("y", &data.orientation.y);
      pRotElement->QueryFloatAttribute("z", &data.orientation.z);
      pRotElement->QueryFloatAttribute("timestamp", &data.timeStamp);
      rotations.push_back(data);
   }

   tinyxml2::XMLElement* pScalesElement = pChannelElement->FirstChildElement("Scales");
   for (tinyxml2::XMLElement* pScaleElement = pScalesElement->FirstChildElement(); pScaleElement; pScaleElement = pScaleElement->NextSiblingElement()) {
      BoneAnimChannel::KeyScale data;

      pScaleElement->QueryFloatAttribute("x", &data.scale.x);
      pScaleElement->QueryFloatAttribute("y", &data.scale.y);
      pScaleElement->QueryFloatAttribute("z", &data.scale.z);
      pScaleElement->QueryFloatAttribute("timestamp", &data.timeStamp);
      scales.push_back(data);
   }

   return BoneAnimChannel(boneName, positions, rotations, scales);
}

static bool animationLoaderReadParams(tinyxml2::XMLElement* const pRoot, float& duration, int& tickPerSecond)
{
   tinyxml2::XMLElement* pParamsElement = pRoot->FirstChildElement("Params");
   if (!pParamsElement) {
      return false;
   }

   pParamsElement->QueryFloatAttribute("duration", &duration);
   pParamsElement->QueryIntAttribute("tickPerSecond", &tickPerSecond);

   return true;
}

bool AnimationResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<AnimationData> pExtra = std::make_shared<AnimationData>();

   tinyxml2::XMLDocument xmlDoc;
   tinyxml2::XMLError error = xmlDoc.Parse(rawBuffer, rawSize);

   if (error != tinyxml2::XMLError::XML_SUCCESS) {
      return false;
   }

   tinyxml2::XMLElement* pRoot = xmlDoc.RootElement();
   if (!pRoot) {
      return false;
   }

   float duration;
   int ticlsPerSecond;
   if (!animationLoaderReadParams(pRoot, duration, ticlsPerSecond)) {
      return false;
   }


   tinyxml2::XMLElement* pBoneChannelsElement = pRoot->FirstChildElement("BoneChannels");
   if (!pBoneChannelsElement) {
      return false;
   }
   std::vector<BoneAnimChannel> boneChannels;

   for (tinyxml2::XMLElement* pBoneChannel = pBoneChannelsElement->FirstChildElement(); pBoneChannel; pBoneChannel = pBoneChannel->NextSiblingElement()) {
      boneChannels.push_back(animationLoaderReadBoneChannel(pBoneChannel));
   }

   pExtra->m_pAnimation = std::make_shared<Animation>(duration, ticlsPerSecond, boneChannels);
   pHandle->SetExtra(pExtra);

   return true;
}


} // namespace BIEngine
