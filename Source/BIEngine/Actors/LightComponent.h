#pragma once

#include "ActorComponent.h"

#include "../Actors/RenderComponent.h"

namespace BIEngine {

class DirectionalLightComponent : public BaseRenderComponent {
public:
   DirectionalLightComponent()
      : BaseRenderComponent(), m_pDirectionalLightNode(nullptr) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return DirectionalLightComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode() override;

protected:
   std::shared_ptr<DirectionalLightNode> m_pDirectionalLightNode;
};

static std::unique_ptr<ActorComponent> CreateDirectionalLightComponent()
{
   return std::make_unique<DirectionalLightComponent>();
}

class PointLightComponent : public BaseRenderComponent {
public:
   PointLightComponent()
      : BaseRenderComponent(), m_pPointLightNode(nullptr) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return PointLightComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode() override;

protected:
   std::shared_ptr<PointLightNode> m_pPointLightNode;
};

static std::unique_ptr<ActorComponent> CreatePointLightComponent()
{
   return std::make_unique<PointLightComponent>();
}

class SpotLightComponent : public BaseRenderComponent {
public:
   SpotLightComponent()
      : BaseRenderComponent(), m_pSpotLightNode(nullptr) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SpotLightComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode() override;

protected:
   std::shared_ptr<SpotLightNode> m_pSpotLightNode;
};

static std::unique_ptr<ActorComponent> CreateSpotLightComponent()
{
   return std::make_unique<SpotLightComponent>();
}
} // namespace BIEngine
