#include "SkinnedMeshComponent.h"

#include "Actor.h"
#include "TransformComponent.h"
#include "SkeletonComponent.h"
#include "../Renderer/SkinnedMeshLoader.h"
#include "../Graphics/MaterialLoader.h"
#include "../Utilities/Logger.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId SkinnedMeshComponent::g_CompId = "SkinnedMeshComponent";

SkinnedMeshComponent::SkinnedMeshComponent()
   : ActorComponent(), m_meshPath(), m_matPath(), m_rootActorPath(),
     m_pMesh(nullptr), m_pMaterial(nullptr), m_pSkeletonRootActor(nullptr)
{
}

bool SkinnedMeshComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pMeshElement = pData->FirstChildElement("Mesh");
   if (pMeshElement) {
      const char* meshPath = nullptr;
      pMeshElement->QueryStringAttribute("path", &meshPath);
      m_meshPath = meshPath;
      auto meshData = StaticPointerCast<SkinnedMeshExtraData>(ResCache::Get()->GetHandle(m_meshPath)->GetExtra());

      if (meshData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading mesh for Actor with id: " + ToString(GetOwner()->GetId()));
         return false;
      }

      m_pMesh = MakeShared<SkinnedMesh>(*meshData->GetSkinnedMesh());
   }

   tinyxml2::XMLElement* pMatElement = pData->FirstChildElement("Material");
   if (pMatElement) {
      const char* matPath = nullptr;
      pMatElement->QueryStringAttribute("path", &matPath);
      m_matPath = matPath;
      auto matData = StaticPointerCast<MaterialData>(ResCache::Get()->GetHandle(m_matPath)->GetExtra());

      if (matData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading mesh material for Actor with id: " + ToString(GetOwner()->GetId()));
         return false;
      }

      m_pMaterial = matData->GetMaterial();
   }

   tinyxml2::XMLElement* pSkeletElement = pData->FirstChildElement("Skelet");
   if (pSkeletElement) {
      const char* skeletPath = nullptr;
      pSkeletElement->QueryStringAttribute("relPath", &skeletPath);
      m_rootActorPath = skeletPath;
   }

   return true;
}

void SkinnedMeshComponent::Activate()
{
   m_pSkeletonRootActor = GetOwner()->GetActorByPath(m_rootActorPath);
}

void SkinnedMeshComponent::OnRenderObject(const GameTimer& gt)
{
   SharedPtr<SkeletonComponent> skeletonComp = m_pSkeletonRootActor->GetComponent<SkeletonComponent>(SkeletonComponent::g_CompId).Lock();
   m_pMesh->OnRender(skeletonComp->GetSkeleton().Get());

   RenderItemsStorage::OpaqueRenderItem opaqueRitem;
   opaqueRitem.actorId = GetOwner()->GetId();
   opaqueRitem.VAO = m_pMesh->GetVao();
   opaqueRitem.IndicesSize = m_pMesh->GetIndices().Size();
   opaqueRitem.pMaterial = m_pMaterial;

   SharedPtr<TransformComponent> pTransformComponent = GetOwner()->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
   opaqueRitem.ModelTransform = pTransformComponent->GetWorldTransformMatrix();

   g_pApp->TryGetHumanView(0)->GetScene()->GetRenderItemsStorage()->InsertOpaqueRenderItem(opaqueRitem);
}

tinyxml2::XMLElement* SkinnedMeshComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());

   tinyxml2::XMLElement* pMeshElement = pDoc->NewElement("Mesh");
   pMeshElement->SetAttribute("path", m_meshPath.CStr());
   pBaseElement->LinkEndChild(pMeshElement);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("path", m_matPath.CStr());
   pBaseElement->LinkEndChild(pMaterialElement);

   tinyxml2::XMLElement* pSkeletElement = pDoc->NewElement("Skelet");
   pSkeletElement->SetAttribute("relPath", m_rootActorPath.CStr());
   pBaseElement->LinkEndChild(pSkeletElement);

   return pBaseElement;
}

} // namespace BIEngine
