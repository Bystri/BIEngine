#include "CombatControllerComponent.h"

#include "../../BIEngine/ProcessManager/ProcessManager.h"
#include "../../BIEngine/ProcessManager/DelayProcess.h"
#include "../../BIEngine/Actors/Actor.h"

const BIEngine::ComponentId CombatControllerComponent::g_CompId = "CombatControllerComponent";

class ActivateDamageTriggerProcess : public BIEngine::Process {
public:
   ActivateDamageTriggerProcess(BIEngine::Actor* pTriggerActor)
      : BIEngine::Process(), m_pTriggerActor(pTriggerActor)
   {
   }

protected:
   virtual void OnUpdate(float dt) override
   {
      m_pTriggerActor->SetActivate(true);
      Succeed();
   }

private:
   BIEngine::Actor* m_pTriggerActor;
};

class DeactivateDamageTriggerProcess : public BIEngine::Process {
public:
   DeactivateDamageTriggerProcess(BIEngine::Actor* pTriggerActor)
      : BIEngine::Process(), m_pTriggerActor(pTriggerActor)
   {
   }

protected:
   virtual void OnUpdate(float dt) override
   {
      m_pTriggerActor->SetActivate(false);
      Succeed();
   }

private:
   BIEngine::Actor* m_pTriggerActor;
};

bool CombatControllerComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to Locomotion Info Component");
   return true;
}

tinyxml2::XMLElement* CombatControllerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void CombatControllerComponent::Activate()
{
   m_pTriggerActor = GetOwner()->GetActorByPath("MeleeAttackTrigger");
   m_pTriggerActor->SetActivate(false);
}

void CombatControllerComponent::Deactivate()
{
   m_pTriggerActor->SetActivate(false);
}

void CombatControllerComponent::OnUpdate(const BIEngine::GameTimer& gt)
{
   BIEngine::SharedPtr<CombatStateComponent> pCombatStateComponent = GetOwner()->GetComponent<CombatStateComponent>(CombatStateComponent::g_CompId).Lock();
   if (pCombatStateComponent->IsAttackInProgress() && m_pAttackProcess.Expired()) {
      pCombatStateComponent->SetIsAttackInProgress(false);
   }
}

void CombatControllerComponent::RequestMeleeAttack()
{
   BIEngine::SharedPtr<CombatStateComponent> pCombatStateComponent = GetOwner()->GetComponent<CombatStateComponent>(CombatStateComponent::g_CompId).Lock();
   if (pCombatStateComponent->IsAttackInProgress()) {
      return;
   }

   BIEngine::Logger::WriteMsgLog("Attack!");

   BIEngine::StrongProcessPtr pStartAttack = BIEngine::MakeShared<BIEngine::DelayProcess>(0.75f);
   BIEngine::StrongProcessPtr pActivateDamagerProcess = BIEngine::MakeShared<ActivateDamageTriggerProcess>(m_pTriggerActor);
   pStartAttack->AttachChild(pActivateDamagerProcess);
   BIEngine::StrongProcessPtr pHitWindow = BIEngine::MakeShared<BIEngine::DelayProcess>(0.5f);
   pActivateDamagerProcess->AttachChild(pHitWindow);
   BIEngine::StrongProcessPtr pDeactivateDamagerProcess = BIEngine::MakeShared<DeactivateDamageTriggerProcess>(m_pTriggerActor);
   pHitWindow->AttachChild(pDeactivateDamagerProcess);
   m_pAttackProcess = pHitWindow;

   BIEngine::ProcessManager::Get()->AttachProcess(pStartAttack);

   pCombatStateComponent->SetIsAttackInProgress(true);
}
