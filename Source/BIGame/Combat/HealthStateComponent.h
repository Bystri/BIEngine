#pragma once

#include "../../BIEngine/Actors/ActorComponent.h"
#include "../../BIEngine/StdLib/UniquePtr.h"

class HealthStateComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;


   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return HealthStateComponent::g_CompId; };

   bool IsDead() const { return m_isDead; }

   void SetIsDead(bool isDead) { m_isDead = isDead; }

   float GetHealth() const { return m_health; }

   void SetHealth(float health) { m_health = health; }

private:
   bool m_isDead = false;
   float m_health = 30.0f;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateHealthComponentComponent()
{
   return BIEngine::MakeUnique<HealthStateComponent>();
}
