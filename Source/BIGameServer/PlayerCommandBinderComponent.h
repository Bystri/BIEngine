#pragma once

#include "../BIEngine/Actors/ActorComponent.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/StdLib/UniquePtr.h"

class PlayerCommandBinderComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual void OnUpdate(const BIEngine::GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return PlayerCommandBinderComponent::g_CompId; };

   void RequestMeleeAttack();

private:
   void HandleOnCommandMoveTo(BIEngine::IEventDataPtr pEventData);

private:
   BIEngine::EventManager::DelegateHandler m_onCommandMoveToHandler;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreatePlayerCommandBinderComponent()
{
   return BIEngine::MakeUnique<PlayerCommandBinderComponent>();
}
