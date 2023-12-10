#pragma once

#include "ActorComponent.h"

#include "../Actors/RenderComponent.h"
#include "../Graphics/LightNodes.h"

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

static ActorComponent* CreateDirectionalLightComponent()
{
   return new DirectionalLightComponent;
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

static ActorComponent* CreatePointLightComponent()
{
   return new PointLightComponent;
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

static ActorComponent* CreateSpotLightComponent()
{
   return new SpotLightComponent;
}
} // namespace BIEngine