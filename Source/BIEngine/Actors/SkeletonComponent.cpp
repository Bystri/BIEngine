#include "SkeletonComponent.h"

#include "Actor.h"
#include "BoneComponent.h"
#include "TransformComponent.h"

namespace BIEngine {

const ComponentId SkeletonComponent::g_CompId = "SkeletonComponent";

SkeletonComponent::SkeletonComponent()
   : ActorComponent(), m_pSkeleton(nullptr)
{
}

bool SkeletonComponent::Init(tinyxml2::XMLElement* pData)
{
   return true;
}

static std::shared_ptr<Skeleton::BoneInfo> skeletonComponentCalculateSkeleton(const glm::mat4& parentTransform, Actor* actor)
{
   std::shared_ptr<BoneComponent> pBoneComp = actor->GetComponent<BoneComponent>(BoneComponent::g_CompId).lock();
   if (pBoneComp == nullptr) {
      return nullptr;
   }

   std::shared_ptr<Skeleton::BoneInfo> pBone = pBoneComp->GetBoneInfo();

   std::shared_ptr<TransformComponent> pTransformComponent = actor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   const glm::mat4 boneTransform = parentTransform * pTransformComponent->GetLocalTransformMatrix();
   pBone->name = actor->GetName();
   pBone->offset = glm::inverse(boneTransform);
   pBone->localTransform = pTransformComponent->GetLocalTransformMatrix();

   for (const auto& child : actor->GetChildren()) {
      std::shared_ptr<Skeleton::BoneInfo> pChildBone = skeletonComponentCalculateSkeleton(boneTransform, child.get());
      if (pChildBone == nullptr) {
         continue;
      }
      pBone->children.push_back(pChildBone);
   }

   return pBone;
}

void SkeletonComponent::Activate()
{
   std::shared_ptr<Skeleton::BoneInfo> pRoot = std::make_shared<Skeleton::BoneInfo>();
   pRoot->name = m_pOwner->GetName();
   pRoot->offset = glm::mat4(1.0f);
   pRoot->localTransform = glm::mat4(1.0f);

   for (const auto& child : m_pOwner->GetChildren()) {
      std::shared_ptr<Skeleton::BoneInfo> pChildBone = skeletonComponentCalculateSkeleton(glm::mat4(1.0f), child.get());
      if (pChildBone == nullptr) {
         continue;
      }
      pRoot->children.push_back(pChildBone);
   }

   m_pSkeleton = std::make_shared<Skeleton>(pRoot);
}

void SkeletonComponent::Deactivate()
{
   m_pSkeleton = nullptr;
}

void SkeletonComponent::OnUpdate(const GameTimer& gt)
{
   m_pSkeleton->Update();
}

tinyxml2::XMLElement* SkeletonComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   return pBaseElement;
}

} // namespace BIEngine
