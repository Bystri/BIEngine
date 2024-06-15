#include "BoneComponent.h"

#include "Actor.h"
#include "TransformComponent.h"

namespace BIEngine {

const ComponentId BoneComponent::g_CompId = "BoneComponent";

BoneComponent::BoneComponent()
   : ActorComponent(), m_pBone(nullptr)
{
}

bool BoneComponent::Init(tinyxml2::XMLElement* pData)
{
   m_pBone = std::make_shared<Skeleton::BoneInfo>();

   return true;
}

void BoneComponent::OnUpdate(const GameTimer& gt)
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   m_pBone->localTransform = pTransformComponent->GetLocalTransformMatrix();
}

tinyxml2::XMLElement* BoneComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   return pBaseElement;
}

} // namespace BIEngine
