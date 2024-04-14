#pragma once

#include <unordered_map>

#include "ActorComponent.h"
#include "../ProcessManager/Process.h"
#include "../Graphics/SkeletalModel.h"

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

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   void PlayAnimation(const std::string& animName);
   void Stop();

private:
   class UpdateAnimProcess : public Process {
   public:
      UpdateAnimProcess(std::shared_ptr<Animator> pAnimator);

   protected:
      virtual void OnUpdate(float dt);

   private:
      std::shared_ptr<Animator> m_pAnimator;
   };

private:
   std::unordered_map<std::string, std::shared_ptr<Animation>> m_animations;
   std::unordered_map<std::string, std::string> m_animationPaths;
   std::shared_ptr<Animator> m_pAnimator;

   std::shared_ptr<UpdateAnimProcess> m_pUpdateAnimProcess;
};

static ActorComponent* CreateAnimatorComponent()
{
   return new AnimationComponent;
}
} // namespace BIEngine
