#include "LightGraphicsTechnique.h"

namespace BIEngine {

LightGraphicsTechnique::~LightGraphicsTechnique()
{
   if (m_lightsBufferData.pDirLights) {
      delete m_lightsBufferData.pDirLights;
      m_lightsBufferData.pDirLights = nullptr;
   }

   if (m_lightsBufferData.pPointLights) {
      delete m_lightsBufferData.pPointLights;
      m_lightsBufferData.pPointLights = nullptr;
   }

   if (m_lightsBufferData.pSpotLights) {
      delete m_lightsBufferData.pSpotLights;
      m_lightsBufferData.pSpotLights = nullptr;
   }
}

bool LightGraphicsTechnique::Init()
{
   m_lightsBufferData.pDirLights = new LightUniformBufferData::DirectionalLightGlData[m_maxDirLightsNum];
   m_lightsBufferData.pPointLights = new LightUniformBufferData::PointLightGlData[m_maxPointLightsNum];
   m_lightsBufferData.pSpotLights = new LightUniformBufferData::SpotLightGlData[m_maxSpotLightsNum];

   // 3 numbers + 1 pan 32bit + light data
   const std::size_t neededSize = sizeof(int32_t) * 4 +
                                  sizeof(LightUniformBufferData::DirectionalLightGlData) * m_maxDirLightsNum +
                                  sizeof(LightUniformBufferData::PointLightGlData) * m_maxPointLightsNum +
                                  sizeof(LightUniformBufferData::SpotLightGlData) * m_maxSpotLightsNum;

   constexpr int CONSTANTS_BUFFER_LIGHTS_BINDING_POINT = 1;

   m_pLightConstantsBuffer->Init(neededSize, CONSTANTS_BUFFER_LIGHTS_BINDING_POINT);

   return true;
}

void LightGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   const auto& dirLights = pStorage->GetDirectionalLightItems();
   const auto& pointLights = pStorage->GetPointLightItems();
   const auto& spotLights = pStorage->GetSpotLightItems();

   m_lightsBufferData.DirectionalLightNum = std::min(m_maxDirLightsNum, dirLights.size());
   for (int i = 0; i < m_lightsBufferData.DirectionalLightNum; ++i) {
      m_lightsBufferData.pDirLights[i] = LightUniformBufferData::DirectionalLightGlData(dirLights[i]);
   }

   m_lightsBufferData.PointLightsNum = std::min(m_maxPointLightsNum, pointLights.size());
   for (int i = 0; i < m_lightsBufferData.PointLightsNum; ++i) {
      m_lightsBufferData.pPointLights[i] = LightUniformBufferData::PointLightGlData(pointLights[i]);
   }

   m_lightsBufferData.SpotLightsNum = std::min(m_maxSpotLightsNum, spotLights.size());
   for (int i = 0; i < m_lightsBufferData.SpotLightsNum; ++i) {
      m_lightsBufferData.pSpotLights[i] = LightUniformBufferData::SpotLightGlData(spotLights[i]);
   }

   std::size_t offset = 0;

   m_pLightConstantsBuffer->SetBufferData(&m_lightsBufferData, offset, 4 * sizeof(int32_t));
   offset += 4 * sizeof(int32_t);

   m_pLightConstantsBuffer->SetBufferData(m_lightsBufferData.pDirLights, offset, sizeof(LightUniformBufferData::DirectionalLightGlData) * m_maxDirLightsNum);
   offset += sizeof(LightUniformBufferData::DirectionalLightGlData) * m_maxDirLightsNum;

   m_pLightConstantsBuffer->SetBufferData(m_lightsBufferData.pPointLights, offset, sizeof(LightUniformBufferData::PointLightGlData) * m_maxPointLightsNum);
   offset += sizeof(LightUniformBufferData::PointLightGlData) * m_maxPointLightsNum;

   m_pLightConstantsBuffer->SetBufferData(m_lightsBufferData.pSpotLights, offset, sizeof(LightUniformBufferData::SpotLightGlData) * m_maxSpotLightsNum);
}

} // namespace BIEngine
