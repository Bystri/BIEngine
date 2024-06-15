#include "AnimationComponent.h"

#include "Actor.h"
#include "RenderComponent.h"
#include "../Graphics/AnimationLoader.h"

namespace BIEngine {

const ComponentId AnimationComponent::g_CompId = "AnimationComponent";

AnimationComponent::AnimationComponent()
   : ActorComponent(), m_animations(), m_pAnimator(nullptr)
{
}

bool AnimationComponent::Init(tinyxml2::XMLElement* pData)
{
   for (tinyxml2::XMLElement* pAnimData = pData->FirstChildElement(); pAnimData; pAnimData = pAnimData->NextSiblingElement()) {
      const char* name;
      const char* animPath;

      pAnimData->QueryStringAttribute("name", &name);
      pAnimData->QueryStringAttribute("path", &animPath);

      auto animationData = std::dynamic_pointer_cast<AnimationData>(ResCache::Get()->GetHandle(animPath)->GetExtra());

      if (animationData == nullptr) {
         return false;
      }

      m_animations[name] = animationData->GetAnimation();
      m_animationPaths[name] = animPath;
   }

   return true;
}

void AnimationComponent::Activate()
{
   m_pAnimator = std::make_shared<Animator>(m_pOwner);
}

void AnimationComponent::Deactivate()
{
   m_pAnimator = nullptr;
}

void AnimationComponent::OnUpdate(const GameTimer& gt)
{
   m_pAnimator->Update(gt.DeltaTime());
}

tinyxml2::XMLElement* AnimationComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   for (const auto& anim : m_animationPaths) {
      tinyxml2::XMLElement* pAnimation = pDoc->NewElement("Animation");
      pAnimation->SetAttribute("name", anim.first.c_str());
      pAnimation->SetAttribute("path", anim.second.c_str());
      pBaseElement->LinkEndChild(pAnimation);
   }

   return pBaseElement;
}

void AnimationComponent::PlayAnimation(const std::string& animName)
{
   m_pAnimator->PlayAnimation(m_animations[animName]);
}

void AnimationComponent::Stop()
{
   m_pAnimator->PlayAnimation(nullptr);
}

} // namespace BIEngine
