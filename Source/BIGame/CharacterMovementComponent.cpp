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

tinyxml2::XMLElement* CharacterMovementComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}

void CharacterMovementComponent::OnFixedUpdate(float dt)
{
   auto pLocomotionInfoComponent = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   const glm::vec3 inputVector = pLocomotionInfoComponent->GetInputVel();
   const glm::vec2 desiredDir = pLocomotionInfoComponent->GetInputDir();

   float orientation = pLocomotionInfoComponent->GetCurrentOrientation();
   float angularVelocity = pLocomotionInfoComponent->GetCurrentAngularVelocity();

   auto pTransformComponent = GetOwner()->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();

   if (glm::length(desiredDir) > std::numeric_limits<float>::epsilon()) {
      float targetAngle = glm::degrees(std::atan2(-desiredDir.y, desiredDir.x));

      constexpr float COMPLETE_ANGLE_DEGREE = 360.0f;
      if (std::abs(targetAngle - orientation) > std::abs(targetAngle + COMPLETE_ANGLE_DEGREE - orientation)) {
         targetAngle += COMPLETE_ANGLE_DEGREE;
      }

      if (std::abs(targetAngle - orientation) > std::abs(targetAngle - COMPLETE_ANGLE_DEGREE - orientation)) {
         targetAngle -= COMPLETE_ANGLE_DEGREE;
      }

      orientation = BIEngine::SmoothDamp(orientation, targetAngle, angularVelocity, m_turnSmoothTime, dt, m_maxAngualerSpeed);
   } else {
       angularVelocity = 0.0f;
   }

   const glm::vec3 charDir = pTransformComponent->GetDir();
   const float orientationRad = orientation * 3.14f / 180.0f;
   const glm::vec2 charDir2d = glm::normalize(glm::vec2(std::cos(orientationRad), std::sin(orientationRad)));
   pLocomotionInfoComponent->SetCurrentDir(charDir2d);
   pLocomotionInfoComponent->SetCurrentOrientation(orientation);
   pLocomotionInfoComponent->SetCurrentAngularVelocity(angularVelocity);

   const glm::vec3 desiredVel = glm::vec3(inputVector.x, 0.0f, inputVector.z) * m_maxSpeed;

   auto pPhysics3DComponent = GetOwner()->GetComponent<BIEngine::Physics3DComponent>(BIEngine::Physics3DComponent::g_CompId).Lock();
   const glm::vec3 curVel = pLocomotionInfoComponent->GetCurrentVel();

   const float maxSpeedChange = m_maxAccelearation * dt;
   const glm::vec3 newVel = glm::vec3(BIEngine::MoveTowards(curVel.x, desiredVel.x, maxSpeedChange), 0.0f, BIEngine::MoveTowards(curVel.z, desiredVel.z, maxSpeedChange));
   const glm::vec3 displ = newVel * dt;
   const glm::vec3 newRotation = glm::vec3(0.0f, orientation, 0.0f);

   if (pPhysics3DComponent->Translate(displ, newRotation)) {
       pLocomotionInfoComponent->SetCurrentVel(newVel);
   }
}
