#pragma once

#include <unordered_map>

#include "ActorComponent.h"
#include "../Graphics/Animator.h"
#include "../ProcessManager/Process.h"

namespace BIEngine {

class AnimationComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return AnimationComponent::g_CompId; };

public:
   AnimationComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual void OnUpdate(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   void PlayAnimation(const std::string& animName);
   void Stop();

private:
   std::unordered_map<std::string, std::shared_ptr<Animation>> m_animations;
   std::unordered_map<std::string, std::string> m_animationPaths;
   std::shared_ptr<Animator> m_pAnimator;
};

static std::unique_ptr<ActorComponent> CreateAnimatorComponent()
{
   return std::make_unique<AnimationComponent>();
}
} // namespace BIEngine
