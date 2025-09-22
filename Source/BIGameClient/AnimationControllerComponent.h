#pragma once

#include "../BIEngine/Actors/ActorComponent.h"
#include "../BIEngine/ProcessManager/Process.h"
#include "../BIEngine/StdLib/UniquePtr.h"

class AnimationControllerComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return AnimationControllerComponent::g_CompId; };

private:
   BIEngine::WeakProcessPtr m_pUpdateAnimProcess;
};

static BIEngine::UniquePtr<BIEngine::ActorComponent> CreateAnimationControllerComponent()
{
   return BIEngine::MakeUnique<AnimationControllerComponent>();
}
