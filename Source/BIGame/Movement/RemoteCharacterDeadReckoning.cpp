#include "RemoteCharacterDeadReckoning.h"

#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/Actors/PlayerComponent.h"
#include "../CharacterMovementComponent.h"
#include "../Network/EventNetworkProtocol.h"
#include "../PlayerManager/PlayerManager.h"

bool RemoteCharacterDeadReckoning::IsRemotePlayerOnClient(BIEngine::Actor& actor)
{
   if (!EventProtocolLeader::Get()) {
      return false;
   }

   auto player = actor.GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock();
   if (!player || PlayerManager::Get()->GetLocalPlayerId() == PlayerManager::INVALID_PLAYER_ID) {
      return false;
   }

   return player->GetPlayerId() != PlayerManager::Get()->GetLocalPlayerId();
}

void RemoteCharacterDeadReckoning::OnSnapshotReceived()
{
   m_hasSnapshot = true;
   m_predictionAge = 0.0f;
}

void RemoteCharacterDeadReckoning::Predict(CharacterMovementComponent& movement, float fixedDt)
{
   constexpr float maxPredictionTime = 0.5f;
   if (!m_hasSnapshot || m_predictionAge >= maxPredictionTime) {
      return;
   }

   const float remainingTime = maxPredictionTime - m_predictionAge;
   const float predictionDt = fixedDt < remainingTime ? fixedDt : remainingTime;
   movement.SimulateInputStep(predictionDt);
   m_predictionAge += predictionDt;
}
