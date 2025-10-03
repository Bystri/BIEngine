#pragma once

#include "../../BIEngine/Actors/ActorComponent.h"
#include "../../BIEngine/StdLib/UniquePtr.h"

class DamagableComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;


   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return DamagableComponent::g_CompId; };

   void TakeDamage(float damage);
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateDamagableComponentComponent()
{
   return BIEngine::MakeUnique<DamagableComponent>();
}
