#pragma once

#include "ActorComponent.h"
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

   std::shared_ptr<Skeleton> GetSkeleton() const { return m_pSkeleton; }

private:
   std::shared_ptr<Skeleton> m_pSkeleton;
};

static std::unique_ptr<ActorComponent> CreateSkeletonComponent()
{
   return std::make_unique<SkeletonComponent>();
}
} // namespace BIEngine
