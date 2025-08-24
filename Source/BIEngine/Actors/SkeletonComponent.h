#pragma once

#include "ActorComponent.h"
#include "../StdLib/UniquePtr.h"
#include "../StdLib/SharedPtr.h"
#include "../Renderer/Skeleton.h"

namespace BIEngine {

class SkeletonComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SkeletonComponent::g_CompId; };

public:
   SkeletonComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual void OnUpdate(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   SharedPtr<Skeleton> GetSkeleton() const { return m_pSkeleton; }

private:
   SharedPtr<Skeleton> m_pSkeleton;
};

static UniquePtr<ActorComponent> CreateSkeletonComponent()
{
   return MakeUnique<SkeletonComponent>();
}
} // namespace BIEngine
