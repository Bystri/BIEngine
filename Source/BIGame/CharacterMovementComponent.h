#pragma once

#include "../BIEngine/Actors/ActorComponent.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/StdLib/UniquePtr.h"
#include "Movement/MovementTypes.h"
#include "Movement/RemoteCharacterDeadReckoning.h"

class CharacterMovementComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return CharacterMovementComponent::g_CompId; };

   virtual void OnFixedUpdate(float dt) override;

   void SimulateInputStep(float dt);
   void SimulateInputStep(const CharacterInputCommand& command, float dt);

   // A replicated remote player may be predicted for a short time after a snapshot.
   void OnRemoteSnapshotReceived();

private:
   RemoteCharacterDeadReckoning m_remotePrediction;

private:
   float m_maxSpeed = 5.0f;
   float m_maxAngualerSpeed = 2000.0f;
   float m_maxAccelearation = 10.0f;
   float m_turnSmoothTime = 0.05f;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateCharacterMovementComponent()
{
   return BIEngine::MakeUnique<CharacterMovementComponent>();
}
