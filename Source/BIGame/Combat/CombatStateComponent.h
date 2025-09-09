#pragma once

#include "../../BIEngine/Actors/ActorComponent.h"
#include "../../BIEngine/StdLib/UniquePtr.h"

class CombatStateComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;


   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return CombatStateComponent::g_CompId; };

   void SetIsAttackInProgress(const bool isInProgress) { m_bIsAttackInProgress = isInProgress; }

   bool IsAttackInProgress() const { return m_bIsAttackInProgress; }

private:
   bool m_bIsAttackInProgress = false;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateCombatStateComponent()
{
   return BIEngine::MakeUnique<CombatStateComponent>();
}
