#include "AnimationControllerComponent.h"

#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Actors/AnimationComponent.h"
#include "../BIEngine/ProcessManager/ProcessManager.h"
#include "../BIGame/Combat/CombatStateComponent.h"
#include "../BIGame/Combat/HealthStateComponent.h"
#include "../BIGame/Locomotion/LocomotionInfoComponent.h"

const BIEngine::ComponentId AnimationControllerComponent::g_CompId = "AnimationControllerComponent";

class AnimationControllerProcess : public BIEngine::Process {
public:
   AnimationControllerProcess(BIEngine::Actor* pAnimatedActor)
      : BIEngine::Process()
   {
      m_pTransformComponent = pAnimatedActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId);
      m_pAnimationComponent = pAnimatedActor->GetComponent<BIEngine::AnimationComponent>(BIEngine::AnimationComponent::g_CompId);
      m_pLocomotionInfoComponent = pAnimatedActor->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId);
      m_pCombatStateComponent = pAnimatedActor->GetComponent<CombatStateComponent>(CombatStateComponent::g_CompId);
      m_pHealthStateComponent = pAnimatedActor->GetComponent<HealthStateComponent>(HealthStateComponent::g_CompId);
   }

protected:
   virtual void OnUpdate(float dt) override
   {
      if (!m_isDead && m_pHealthStateComponent.Lock()->IsDead()) {
         m_pAnimationComponent.Lock()->PlayAnimation("death");
         m_isDead = true;
      }

      if (m_isDead) {
         return;
      }

      auto pLocomotionInfoComponent = m_pLocomotionInfoComponent.Lock();
      glm::vec2 currentVel2d = glm::vec2(pLocomotionInfoComponent->GetCurrentVel().x, pLocomotionInfoComponent->GetCurrentVel().z);

      if (m_isAttackInProgress && !m_pCombatStateComponent.Lock()->IsAttackInProgress()) {
         m_isAttackInProgress = false;

         if (glm::length(currentVel2d) < 0.001) {
            m_pAnimationComponent.Lock()->PlayAnimation("idle");
            return;
         }
      }

      if (!m_isAttackInProgress && m_pCombatStateComponent.Lock()->IsAttackInProgress()) {
         m_pAnimationComponent.Lock()->PlayAnimation("2H_Melee_Attack_Chop");

         m_isAttackInProgress = true;

         m_isRunningFront = false;
         m_isRunningBack = false;
         m_isRunningRight = false;
         m_isRunningLeft = false;
      }

      if (m_isAttackInProgress) {
         return;
      }

      if (glm::length(currentVel2d) > 0.001 && !m_pCombatStateComponent.Lock()->IsAttackInProgress()) {
         currentVel2d = glm::normalize(currentVel2d);
         const float dirDot = glm::dot(pLocomotionInfoComponent->GetCurrentDir(), currentVel2d);

         const glm::vec3 right3d = m_pTransformComponent.Lock()->GetRight();
         const glm::vec2 right2d = glm::vec2(right3d.x, right3d.z);
         glm::normalize(right2d);

         const float rightDot = glm::dot(currentVel2d, right2d);
         if (rightDot > 0.707f) {
            if (m_isRunningFront) {
               return;
            }

            m_pAnimationComponent.Lock()->PlayAnimation("run");

            m_isRunningFront = true;
            m_isRunningBack = false;
            m_isRunningRight = false;
            m_isRunningLeft = false;
         } else if (dirDot < -0.707f) {
            if (m_isRunningBack) {
               return;
            }

            m_pAnimationComponent.Lock()->PlayAnimation("walking_backwards");

            m_isRunningFront = false;
            m_isRunningBack = true;
            m_isRunningRight = false;
            m_isRunningLeft = false;
         } else if (rightDot > 0.707f) {
            if (m_isRunningRight) {
               return;
            }

            m_pAnimationComponent.Lock()->PlayAnimation("strafe_right");

            m_isRunningFront = false;
            m_isRunningBack = false;
            m_isRunningRight = true;
            m_isRunningLeft = false;
         } else {
            if (m_isRunningLeft) {
               return;
            }

            m_pAnimationComponent.Lock()->PlayAnimation("strafe_left");

            m_isRunningFront = false;
            m_isRunningBack = false;
            m_isRunningRight = false;
            m_isRunningLeft = true;
         }
      } else {
         if (m_isRunningFront || m_isRunningBack || m_isRunningRight || m_isRunningLeft) {
            m_pAnimationComponent.Lock()->PlayAnimation("idle");

            m_isRunningFront = false;
            m_isRunningBack = false;
            m_isRunningRight = false;
            m_isRunningLeft = false;
         }
      }
   }

private:
   BIEngine::WeakPtr<BIEngine::TransformComponent> m_pTransformComponent;
   BIEngine::WeakPtr<BIEngine::AnimationComponent> m_pAnimationComponent;
   BIEngine::WeakPtr<LocomotionInfoComponent> m_pLocomotionInfoComponent;
   BIEngine::WeakPtr<CombatStateComponent> m_pCombatStateComponent;
   BIEngine::WeakPtr<HealthStateComponent> m_pHealthStateComponent;

   bool m_isRunningFront = true;
   bool m_isRunningBack = true;
   bool m_isRunningRight = true;
   bool m_isRunningLeft = true;
   bool m_isAttackInProgress = true;
   bool m_isDead = false;
};

bool AnimationControllerComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to AnimationControllerComponent");
   return true;
}

tinyxml2::XMLElement* AnimationControllerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void AnimationControllerComponent::Activate()
{
   BIEngine::StrongProcessPtr pUpdateAnimationProcess = BIEngine::MakeShared<AnimationControllerProcess>(GetOwner());
   BIEngine::ProcessManager::Get()->AttachProcess(pUpdateAnimationProcess);
   m_pUpdateAnimProcess = pUpdateAnimationProcess;
}

void AnimationControllerComponent::Deactivate()
{
   m_pUpdateAnimProcess.Lock()->Succeed();
}
