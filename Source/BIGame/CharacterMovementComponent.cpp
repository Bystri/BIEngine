#include "CharacterMovementComponent.h"

#include "Movement/CharacterMovementSimulator.h"
#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Actors/Physics3DComponent.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIGame/Locomotion/LocomotionInfoComponent.h"
#include "PlayerCommandBinderComponent.h"
#include "PlayerManager/PlayerManager.h"
#include "Network/EventNetworkProtocol.h"
#include "../BIEngine/Actors/PlayerComponent.h"

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

   if (IsRemotePlayerOnClient()) {
      constexpr float maxPredictionTime = 0.5f;
      if (m_hasRemoteSnapshot && m_remotePredictionAge < maxPredictionTime) {
         const float remainingTime = maxPredictionTime - m_remotePredictionAge;
         const float predictionDt = dt < remainingTime ? dt : remainingTime;
         SimulateInputStep(predictionDt);
         m_remotePredictionAge += predictionDt;
      }
      return;
   }

   // On the server a delayed packet can contain several fixed-step commands.
   CharacterInputCommand command;
   while (binder->PopNextCharacterInput(command)) {
      SimulateInputStep(command, dt);
   }
}

bool CharacterMovementComponent::IsRemotePlayerOnClient() const
{
   if (!EventProtocolLeader::Get()) {
      return false;
   }

   auto player = GetOwner()->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock();
   if (!player || PlayerManager::Get()->GetLocalPlayerId() == PlayerManager::INVALID_PLAYER_ID) {
      return false;
   }

   return player->GetPlayerId() != PlayerManager::Get()->GetLocalPlayerId();
}

void CharacterMovementComponent::OnRemoteSnapshotReceived()
{
   m_hasRemoteSnapshot = true;
   m_remotePredictionAge = 0.0f;
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
