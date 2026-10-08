#pragma once

namespace BIEngine {
class Actor;
}

class CharacterMovementComponent;

class RemoteCharacterDeadReckoning {
public:
   static bool IsRemotePlayerOnClient(BIEngine::Actor& actor);

   void OnSnapshotReceived();
   void Predict(CharacterMovementComponent& movement, float fixedDt);

private:
   float m_predictionAge = 0.0f;
   bool m_hasSnapshot = false;
};
