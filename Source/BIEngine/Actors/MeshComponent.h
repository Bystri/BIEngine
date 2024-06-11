#pragma once

#include "ActorComponent.h"

namespace BIEngine {

class Mesh;
class Material;

class MeshComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return MeshComponent::g_CompId; };

public:
   MeshComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

private:
   std::string m_meshPath;
   std::string m_matPath;

   std::shared_ptr<Mesh> m_pMesh;
   std::shared_ptr<Material> m_pMaterial;
};

static std::unique_ptr<ActorComponent> CreateMeshComponent()
{
   return std::make_unique<MeshComponent>();
}

} // namespace BIEngine
