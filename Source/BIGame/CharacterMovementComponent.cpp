#include "CharacterMovementComponent.h"

#include "Movement/CharacterMovementSimulator.h"
#include "Movement/ServerCharacterInputProcessor.h"
#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Actors/Physics3DComponent.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIGame/Locomotion/LocomotionInfoComponent.h"
#include "PlayerCommandBinderComponent.h"

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
   auto binder = GetOwner()->GetComponent<PlayerCommandBinderComponent>(PlayerCommandBinderComponent::g_CompId).Lock();
   if (!binder) {
      SimulateInputStep(dt);
      return;
   }

   if (RemoteCharacterDeadReckoning::IsRemotePlayerOnClient(*GetOwner())) {
      m_remotePrediction.Predict(*this, dt);
      return;
   }

   ServerCharacterInputProcessor::ProcessPending(binder->GetInputQueue(), *this, dt);
}

void CharacterMovementComponent::OnRemoteSnapshotReceived()
{
   m_remotePrediction.OnSnapshotReceived();
}

void CharacterMovementComponent::SimulateInputStep(float dt)
{
   auto locomotion = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   CharacterInputCommand command;
   command.inputVelocity = locomotion->GetInputVel();
   command.inputDirection = locomotion->GetInputDir();
   SimulateInputStep(command, dt);
}

void CharacterMovementComponent::SimulateInputStep(const CharacterInputCommand& command, float dt)
{
   auto locomotion = GetOwner()->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   auto transform = GetOwner()->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
   auto physics = GetOwner()->GetComponent<BIEngine::Physics3DComponent>(BIEngine::Physics3DComponent::g_CompId).Lock();

   CharacterMovementState previous;
   previous.position = transform->GetPosition();
   previous.rotation = transform->GetRotation();
   previous.velocity = locomotion->GetCurrentVel();
   previous.direction = locomotion->GetCurrentDir();
   previous.inputVelocity = locomotion->GetInputVel();
   previous.inputDirection = locomotion->GetInputDir();
   previous.orientation = locomotion->GetCurrentOrientation();
   previous.angularVelocity = locomotion->GetCurrentAngularVelocity();

   const CharacterMovementConfig config{m_maxSpeed, m_maxAngualerSpeed, m_maxAccelearation, m_turnSmoothTime};
   const CharacterMovementStep step = CharacterMovementSimulator::Step(previous, command, config, dt);
   locomotion->SetInputVel(step.state.inputVelocity);
   locomotion->SetInputDir(step.state.inputDirection);
   locomotion->SetCurrentDir(step.state.direction);
   locomotion->SetCurrentOrientation(step.state.orientation);
   locomotion->SetCurrentAngularVelocity(step.state.angularVelocity);
   if (physics->Translate(step.displacement, step.state.rotation)) {
      locomotion->SetCurrentVel(step.state.velocity);
   }
}
