#include "CombatControllerComponent.h"

#include "../../BIEngine/ProcessManager/ProcessManager.h"
#include "../../BIEngine/ProcessManager/DelayProcess.h"
#include "../../BIEngine/Physics/Physics3DEventListener.h"
#include "../../BIEngine/EngineCore/GameApp.h"
#include "../../BIEngine/Actors/Actor.h"
#include "DamagableComponent.h"

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

   m_handleCollisionDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(CombatControllerComponent::HandleOnTriggerEnter), BIEngine::EvtData_Phys3DTrigger_Enter::sk_EventType);
}

void CombatControllerComponent::Deactivate()
{
   m_pTriggerActor->SetActivate(false);
}

void CombatControllerComponent::HandleOnTriggerEnter(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<BIEngine::EvtData_Phys3DTrigger_Enter> pCastEventData = BIEngine::StaticPointerCast<BIEngine::EvtData_Phys3DTrigger_Enter>(pEventData);
   if (pCastEventData->GetTriggerId() != m_pTriggerActor->GetId()) {
      return;
   }

   if (pCastEventData->GetOtherActor() == GetOwner()->GetId()) {
      return;
   }

   BIEngine::SharedPtr<BIEngine::Actor> pAttackedActor = BIEngine::g_pApp->m_pGameLogic->GetActor(pCastEventData->GetOtherActor());

   auto pDamagableComponent = pAttackedActor->GetComponent<DamagableComponent>(DamagableComponent::g_CompId);
   if (pDamagableComponent.Expired()) {
      return;
   }

   pDamagableComponent.Lock()->TakeDamage(m_damageAmount);
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

   BIEngine::Logger::WriteMsgLog("%s perform melee attack!", GetOwner()->GetName().CStr());

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
