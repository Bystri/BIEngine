#pragma once

#include "ActorComponent.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/UniquePtr.h"
#include "../Renderer/Skeleton.h"

namespace BIEngine {

class BoneComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return BoneComponent::g_CompId; };

public:
   BoneComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void OnUpdate(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   SharedPtr<Skeleton::BoneInfo> GetBoneInfo() const { return m_pBone; }

private:
   SharedPtr<Skeleton::BoneInfo> m_pBone;
};

static UniquePtr<ActorComponent> CreateBoneComponent()
{
   return MakeUnique<BoneComponent>();
}
} // namespace BIEngine
