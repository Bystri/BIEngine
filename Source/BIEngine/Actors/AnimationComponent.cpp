#include "AnimationComponent.h"

#include "Actor.h"
#include "RenderComponent.h"
#include "../Graphics/AnimationLoader.h"
#include "../ProcessManager/ProcessManager.h"

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
   auto pWeakModelComponent = m_pOwner->GetComponent<SkeletalModelRenderComponent>(SkeletalModelRenderComponent::g_CompId);
   auto pModelComponent = pWeakModelComponent.lock();

   if (!pModelComponent) {
      Assert(false, "Need SkeletalModelRenderComponent for AnimationComponent!");
      return;
   }

   m_pAnimator = std::make_shared<Animator>(pModelComponent->GetModel());

   m_pUpdateAnimProcess = std::make_shared<UpdateAnimProcess>(m_pAnimator);
   ProcessManager::Get()->AttachProcess(m_pUpdateAnimProcess);
}

void AnimationComponent::Deactivate()
{
   m_pUpdateAnimProcess->Succeed();
   m_pUpdateAnimProcess = nullptr;
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

AnimationComponent::UpdateAnimProcess::UpdateAnimProcess(std::shared_ptr<Animator> pAnimator)
   : m_pAnimator(pAnimator)
{
}

void AnimationComponent::UpdateAnimProcess::OnUpdate(float dt)
{
   m_pAnimator->Update(dt);
}

} // namespace BIEngine
