#pragma once

#include "../BIEngine/Actors/ActorComponent.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/StdLib/UniquePtr.h"
#include "Movement/ServerCharacterInputQueue.h"

class EvtData_CharacterInput;

class PlayerCommandBinderComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return PlayerCommandBinderComponent::g_CompId; };

   void RequestMeleeAttack();

   ServerCharacterInputQueue& GetInputQueue() { return m_inputQueue; }
   uint32_t GetLastProcessedInputSequence() const { return m_inputQueue.GetLastProcessedSequence(); }

private:
   void HandleOnCommandCharacterInput(BIEngine::IEventDataPtr pEventData);

private:
   BIEngine::EventManager::DelegateHandler m_onCommandCharacterInput;
   ServerCharacterInputQueue m_inputQueue;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreatePlayerCommandBinderComponent()
{
   return BIEngine::MakeUnique<PlayerCommandBinderComponent>();
}
