#pragma once

#include <glm/glm.hpp>

#include "Model.h"
#include "../Actors/Actor.h"
#include "../Renderer/Color.h"
#include "../StdLib/SharedPtr.h"

namespace BIEngine {

class RenderItemsStorage {
public:
   struct OpaqueRenderItem {
      ActorId actorId;
      unsigned int VAO;
      std::size_t IndicesSize;
      SharedPtr<Material> pMaterial;
      glm::mat4 ModelTransform;
   };

   struct DirectionalLightItem {
      glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);
      float irradiance = 1.0f;
      ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);

      // Shadow data
      glm::mat4 LightMatr;
      SharedPtr<Texture2D> pShadowMap;
   };

   struct PointLightItem {
      glm::vec3 position = glm::vec3(0.0f);
      float intensity = 1.0f;
      ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);

      // Shadow data
      SharedPtr<CubemapTexture> pShadowMap;
   };

   struct SpotLightItem {
      glm::vec3 position = glm::vec3(0.0f);
      glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

      ColorRgb ambient = ColorRgb(0.05f, 0.05f, 0.05f);
      ColorRgb diffuse = ColorRgb(1.0f, 1.0f, 1.0f);
      ColorRgb specular = ColorRgb(1.0f, 1.0f, 1.0f);

      float constant = 1.0f;
      float linear = 0.09f;
      float outerCutOff = glm::cos(glm::radians(25.0f));
      float cutOff = glm::cos(glm::radians(12.5f));
      float quadratic = 0.032f;
   };

   void InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem);

   DynamicArray<OpaqueRenderItem>& GetOpaqueRenderItems() { return m_opaqueItems; }

   void InsertDirectionalLightInfo(const DirectionalLightItem& dirLight);
   void InsertPointLightInfo(const PointLightItem& pointLight);
   void InsertSpotLightInfo(const SpotLightItem& spotLight);

   DynamicArray<DirectionalLightItem>& GetDirectionalLightItems() { return m_directionalLightItems; }

   DynamicArray<PointLightItem>& GetPointLightItems() { return m_pointLightItems; }

   DynamicArray<SpotLightItem>& GetSpotLightItems() { return m_spotLightItems; }

   void Clear();

private:
   DynamicArray<OpaqueRenderItem> m_opaqueItems;

   DynamicArray<DirectionalLightItem> m_directionalLightItems;
   DynamicArray<PointLightItem> m_pointLightItems;
   DynamicArray<SpotLightItem> m_spotLightItems;
};

} // namespace BIEngine
