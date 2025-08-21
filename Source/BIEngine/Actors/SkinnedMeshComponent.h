#pragma once

#include <glm/glm.hpp>

#include "../StdLib/UniquePtr.h"
#include "ActorComponent.h"
#include "../Renderer/Skeleton.h"

namespace BIEngine {

class SkinnedMesh;
class Material;

class SkinnedMeshComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SkinnedMeshComponent::g_CompId; };

public:
   SkinnedMeshComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void Activate() override;

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

private:
   String m_meshPath;
   String m_matPath;
   String m_rootActorPath;

   std::shared_ptr<SkinnedMesh> m_pMesh;
   std::shared_ptr<Material> m_pMaterial;
   Actor* m_pSkeletonRootActor;
};

static UniquePtr<ActorComponent> CreateSkinnedMeshComponent()
{
   return MakeUnique<SkinnedMeshComponent>();
}

} // namespace BIEngine
