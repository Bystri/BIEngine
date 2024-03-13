#pragma once

#include "GraphicsTechnique.h"

namespace BIEngine {

class LightGraphicsTechnique : public IGraphicsTechnique {
public:
   LightGraphicsTechnique(const std::size_t maxDirLightsNum, const std::size_t maxPointLightsNum, const std::size_t maxSpotLightsNum)
      : IGraphicsTechnique(),
        m_maxDirLightsNum(maxDirLightsNum), m_maxPointLightsNum(maxPointLightsNum), m_maxSpotLightsNum(maxSpotLightsNum),
        m_lightsBufferData(), m_pLightConstantsBuffer(std::make_shared<ConstantsBuffer>())
   {
   }

   ~LightGraphicsTechnique();

   bool Init() override;
   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   struct LightUniformBufferData {
      struct DirectionalLightGlData {
         DirectionalLightGlData() = default;

         DirectionalLightGlData(const RenderItemsStorage::DirectionalLightItem& dirLight)
            : direction(dirLight.direction), irradiance(dirLight.irradiance), color(dirLight.color)
         {
         }

         glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);
         float irradiance = 1.0f;
         ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);
         uint32_t pan2 = 0u;
      };

      struct PointLightGlData {
         PointLightGlData() = default;

         PointLightGlData(const RenderItemsStorage::PointLightItem& pointLight)
            : position(pointLight.position), intensity(pointLight.intensity), color(pointLight.color)
         {
         }

         glm::vec3 position = glm::vec3(0.0f);
         float intensity = 1.0f;
         ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);
         uint32_t pan7 = 0u;
      };

      struct SpotLightGlData {
         SpotLightGlData() = default;

         SpotLightGlData(const RenderItemsStorage::SpotLightItem& spotLight)
            : position(spotLight.position), direction(spotLight.direction),
              ambient(spotLight.ambient), diffuse(spotLight.diffuse), specular(spotLight.specular),
              constant(spotLight.constant), linear(spotLight.linear), quadratic(spotLight.quadratic),
              cutOff(spotLight.cutOff), outerCutOff(spotLight.outerCutOff)
         {
         }

         glm::vec3 position = glm::vec3(0.0f);
         float constant = 1.0f;
         glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
         float linear = 0.09f;
         ColorRgb ambient = ColorRgb(0.05f, 0.05f, 0.05f);
         float quadratic = 0.032f;
         ColorRgb diffuse = ColorRgb(1.0f, 1.0f, 1.0f);
         float cutOff = glm::cos(glm::radians(12.5f));
         ColorRgb specular = ColorRgb(1.0f, 1.0f, 1.0f);
         float outerCutOff = glm::cos(glm::radians(25.0f));
      };

      int32_t DirectionalLightNum;
      int32_t PointLightsNum;
      int32_t SpotLightsNum;
      uint32_t pan1;
      DirectionalLightGlData* pDirLights = nullptr;
      PointLightGlData* pPointLights = nullptr;
      SpotLightGlData* pSpotLights = nullptr;
   };

private:
   const std::size_t m_maxDirLightsNum;
   const std::size_t m_maxPointLightsNum;
   const std::size_t m_maxSpotLightsNum;

   LightUniformBufferData m_lightsBufferData;
   std::shared_ptr<ConstantsBuffer> m_pLightConstantsBuffer;
};

} // namespace BIEngine
