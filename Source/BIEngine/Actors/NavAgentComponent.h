﻿#pragma once

#include "ActorComponent.h"
#include "../Navigation/NavWorld.h"

namespace BIEngine {

class NavAgentComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return NavAgentComponent::g_CompId; };

public:
   NavAgentComponent();
   virtual ~NavAgentComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void PostInit() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   bool SetDestination(const glm::vec3& pos);
   glm::vec3 GetVelocity() const;

private:
   std::shared_ptr<NavCrowd> m_pNavCrowd;
   NavAgentId m_agentId;
};

static ActorComponent* CreateNavAgentComponent()
{
   return new NavAgentComponent;
}
} // namespace BIEngine