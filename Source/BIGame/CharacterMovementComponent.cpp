#include "CharacterMovementComponent.h"

#include "../BIEngine/Math/Math.h"
#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Actors/Physics3DComponent.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIGame/Locomotion/LocomotionInfoComponent.h"

const BIEngine::ComponentId CharacterMovementComponent::g_CompId = "CharacterMovementComponent";

bool CharacterMovementComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to CharacterMovementComponent");
   return true;
}

void CharacterMovementComponent::Activate()
{
   auto pTransformComponent = GetOwner()->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
   m_orientation = pTransformComponent->GetRotation().y;
}

tinyxml2::XMLElement* CharacterMovementComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void CharacterMovementComponent::OnUpdate(const BIEngine::GameTimer& gt)
{
   auto pLocomotionInfoComponent = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   const glm::vec3 inputVector = pLocomotionInfoComponent->GetInputVel();
   const glm::vec2 desiredDir = pLocomotionInfoComponent->GetInputDir();

   auto pTransformComponent = GetOwner()->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();

   if (glm::length(desiredDir) > std::numeric_limits<float>::epsilon()) {
      float targetAngle = glm::degrees(std::atan2(-desiredDir.y, desiredDir.x));

      constexpr float COMPLETE_ANGLE_DEGREE = 360.0f;
      if (std::abs(targetAngle - m_orientation) > std::abs(targetAngle + COMPLETE_ANGLE_DEGREE - m_orientation)) {
         targetAngle += COMPLETE_ANGLE_DEGREE;
      }

      if (std::abs(targetAngle - m_orientation) > std::abs(targetAngle - COMPLETE_ANGLE_DEGREE - m_orientation)) {
         targetAngle -= COMPLETE_ANGLE_DEGREE;
      }

      m_orientation = BIEngine::SmoothDamp(m_orientation, targetAngle, m_turnSmoothVelocity, m_turnSmoothTime, gt.DeltaTime(), m_maxAngualerSpeed);
      pTransformComponent->SetRotation(glm::vec3(0.0f, m_orientation, 0.0f));
   } else {
      m_turnSmoothVelocity = 0.0f;
   }

   const glm::vec3 charDir = pTransformComponent->GetDir();
   const glm::vec2 charDir2d = glm::normalize(glm::vec2(charDir.x, charDir.z));
   pLocomotionInfoComponent->SetCurrentDir(charDir2d);

   const glm::vec3 desiredVel = glm::vec3(inputVector.x, 0.0f, inputVector.z) * m_maxSpeed;

   auto pPhysics3DComponent = GetOwner()->GetComponent<BIEngine::Physics3DComponent>(BIEngine::Physics3DComponent::g_CompId).Lock();
   const glm::vec3 curVel = pPhysics3DComponent->GetVelocity();
   pLocomotionInfoComponent->SetCurrentVel(curVel);

   const float maxSpeedChange = m_maxAccelearation * gt.DeltaTime();
   const glm::vec3 newVel = glm::vec3(BIEngine::MoveTowards(curVel.x, desiredVel.x, maxSpeedChange), 0.0f, BIEngine::MoveTowards(curVel.z, desiredVel.z, maxSpeedChange));
   pPhysics3DComponent->SetVelocity(newVel);
}
