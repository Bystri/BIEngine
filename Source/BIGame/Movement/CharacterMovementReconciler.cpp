#include "CharacterMovementReconciler.h"

#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/Actors/Physics3DComponent.h"
#include "../../BIEngine/Actors/TransformComponent.h"
#include "../../BIEngine/EngineCore/GameApp.h"
#include "../CharacterMovementComponent.h"
#include "../Locomotion/LocomotionInfoComponent.h"

bool CharacterMovementReconciler::Reconcile(BIEngine::Actor& actor,
                                             const CharacterMovementSnapshot& snapshot,
                                             CharacterInputHistory& history,
                                             float fixedDt)
{
   auto transform = actor.GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
   auto locomotion = actor.GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
   auto movement = actor.GetComponent<CharacterMovementComponent>(CharacterMovementComponent::g_CompId).Lock();
   if (!transform || !locomotion || !movement || !history.Acknowledge(snapshot.lastProcessedSequence)) {
      return false;
   }

   transform->SetPosition(snapshot.state.position);
   transform->SetRotation(snapshot.state.rotation);
   BIEngine::g_pApp->m_pGameLogic->GetGamePhysics3D()->SetPosition(actor.GetId(), snapshot.state.position);
   auto physics = actor.GetComponent<BIEngine::Physics3DComponent>(BIEngine::Physics3DComponent::g_CompId).Lock();
   if (physics) {
      physics->Translate(glm::vec3(0.0f), snapshot.state.rotation);
   }
   locomotion->SetCurrentVel(snapshot.state.velocity);
   locomotion->SetCurrentDir(snapshot.state.direction);
   locomotion->SetInputVel(snapshot.state.inputVelocity);
   locomotion->SetInputDir(snapshot.state.inputDirection);
   locomotion->SetCurrentOrientation(snapshot.state.orientation);
   locomotion->SetCurrentAngularVelocity(snapshot.state.angularVelocity);

   for (const auto& command : history.Pending()) {
      movement->SimulateInputStep(command, fixedDt);
   }
   return true;
}
