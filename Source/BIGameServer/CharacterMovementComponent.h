#pragma once

#include "../BIEngine/Actors/ActorComponent.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/StdLib/UniquePtr.h"

class CharacterMovementComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return CharacterMovementComponent::g_CompId; };

   virtual void OnUpdate(const BIEngine::GameTimer& gt) override;

private:
   float m_maxSpeed = 5.0f;
   float m_maxAngualerSpeed = 2000.0f;
   float m_maxAccelearation = 10.0f;
   float m_turnSmoothTime = 0.05f;
   float m_turnSmoothVelocity = 0.0f;

   float m_orientation = 0.0f;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateCharacterMovementComponent()
{
   return BIEngine::MakeUnique<CharacterMovementComponent>();
}
