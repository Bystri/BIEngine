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
   m_pBone = MakeShared<Skeleton::BoneInfo>();

   return true;
}

void BoneComponent::OnUpdate(const GameTimer& gt)
{
   SharedPtr<TransformComponent> pTransformComponent = GetOwner()->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
   m_pBone->localTransform = pTransformComponent->GetLocalTransformMatrix();
}

tinyxml2::XMLElement* BoneComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());

   return pBaseElement;
}

} // namespace BIEngine
