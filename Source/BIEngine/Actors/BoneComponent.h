#pragma once

#include <unordered_map>

#include "ActorComponent.h"
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

   std::shared_ptr<Skeleton::BoneInfo> GetBoneInfo() const { return m_pBone; }

private:
   std::shared_ptr<Skeleton::BoneInfo> m_pBone;
};

static std::unique_ptr<ActorComponent> CreateBoneComponent()
{
   return std::make_unique<BoneComponent>();
}
} // namespace BIEngine
