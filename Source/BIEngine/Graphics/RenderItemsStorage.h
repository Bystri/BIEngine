#pragma once

#include <queue>
#include <memory>

#include <glm/glm.hpp>

#include "Model.h"
#include "SkeletalModel.h"
#include "../Actors/Actor.h"
#include "../Renderer/Color.h"

namespace BIEngine {

class RenderItemsStorage {
public:
   struct OpaqueRenderItem {
      ActorId actorId;
      std::shared_ptr<Mesh> pMesh;
      std::shared_ptr<Material> pMaterial;
      glm::mat4 ModelTransform;
   };

   struct OpaqueAnimatedRenderItem {
      ActorId actorId;
      std::shared_ptr<SkeletalMesh> pMesh;
      std::shared_ptr<Material> pMaterial;
      glm::mat4 ModelTransform;
      std::vector<glm::mat4> boneMatrices;
   };

   struct DirectionalLightItem {
      glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);
      float irradiance = 1.0f;
      ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);

      // Shadow data
      glm::mat4 LightMatr;
      std::shared_ptr<Texture2D> pShadowMap;
   };

   struct PointLightItem {
      glm::vec3 position = glm::vec3(0.0f);
      float intensity = 1.0f;
      ColorRgb color = ColorRgb(0.75f, 0.75f, 0.05f);

      // Shadow data
      std::shared_ptr<CubemapTexture> pShadowMap;
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

   std::vector<OpaqueRenderItem>& GetOpaqueRenderItems() { return m_opaqueItems; }

   void InsertOpaqueAnimatedRenderItem(const OpaqueAnimatedRenderItem& opaqueRitem);

   std::vector<OpaqueAnimatedRenderItem>& GetOpaqueAnimatedRenderItems() { return m_opaqueAnimatedItems; }

   void InsertDirectionalLightInfo(const DirectionalLightItem& dirLight);
   void InsertPointLightInfo(const PointLightItem& pointLight);
   void InsertSpotLightInfo(const SpotLightItem& spotLight);

   std::vector<DirectionalLightItem>& GetDirectionalLightItems() { return m_directionalLightItems; }

   std::vector<PointLightItem>& GetPointLightItems() { return m_pointLightItems; }

   std::vector<SpotLightItem>& GetSpotLightItems() { return m_spotLightItems; }

   void Clear();

private:
   std::vector<OpaqueRenderItem> m_opaqueItems;
   std::vector<OpaqueAnimatedRenderItem> m_opaqueAnimatedItems;

   std::vector<DirectionalLightItem> m_directionalLightItems;
   std::vector<PointLightItem> m_pointLightItems;
   std::vector<SpotLightItem> m_spotLightItems;
};

} // namespace BIEngine
