#pragma once

#include <glm/glm.hpp>

#include "ActorComponent.h"
#include "../Renderer/Color.h"

namespace BIEngine {

class DirectionalLightComponent : public ActorComponent {
public:
   DirectionalLightComponent()
      : ActorComponent(), m_irradiance(1.0f), m_color(0.75f, 0.75f, 0.05f)
   {
   }

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return DirectionalLightComponent::g_CompId; }

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData) override;

protected:
   float m_irradiance;
   ColorRgb m_color;
};

static std::unique_ptr<ActorComponent> CreateDirectionalLightComponent()
{
   return std::make_unique<DirectionalLightComponent>();
}

class PointLightComponent : public ActorComponent {
public:
   PointLightComponent()
      : ActorComponent(), m_intensity(1.0f), m_color(0.75f, 0.75f, 0.05f) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return PointLightComponent::g_CompId; }

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData) override;

protected:
   float m_intensity;
   ColorRgb m_color;
};

static std::unique_ptr<ActorComponent> CreatePointLightComponent()
{
   return std::make_unique<PointLightComponent>();
}

class SpotLightComponent : public ActorComponent {
public:
   SpotLightComponent()
      : ActorComponent(),
        m_attenuationConstant(1.0f), m_attenuationLinear(0.09f), m_attenuationQuadratic(0.032f),
        m_ambient(0.05f, 0.05f, 0.05f), m_diffuse(0.5f, 0.5f, 0.5f), m_specular(1.0f, 1.0f, 1.0f),
        m_cutOff(glm::cos(glm::radians(12.5f))), m_outerCutOff(glm::cos(glm::radians(25.0f)))
   {
   }

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SpotLightComponent::g_CompId; }

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData) override;

protected:
   float m_attenuationConstant;
   float m_attenuationLinear;
   float m_attenuationQuadratic;

   ColorRgb m_ambient;
   ColorRgb m_diffuse;
   ColorRgb m_specular;

   float m_cutOff;
   float m_outerCutOff;
};

static std::unique_ptr<ActorComponent> CreateSpotLightComponent()
{
   return std::make_unique<SpotLightComponent>();
}
} // namespace BIEngine
