#pragma once

#include "../../BIEngine/Actors/ActorComponent.h"
#include "../../BIEngine/ProcessManager/Process.h"
#include "../../BIEngine/StdLib/UniquePtr.h"

#include "CombatStateComponent.h"

class CombatControllerComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual void OnUpdate(const BIEngine::GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return CombatControllerComponent::g_CompId; };

   void RequestMeleeAttack();

private:
   BIEngine::Actor* m_pTriggerActor = nullptr;
   BIEngine::WeakProcessPtr m_pAttackProcess;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateCombatControllerComponent()
{
   return BIEngine::MakeUnique<CombatControllerComponent>();
}
