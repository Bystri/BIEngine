#include "MeshComponent.h"

#include "Actor.h"
#include "TransformComponent.h"
#include "../Renderer/MeshLoader.h"
#include "../Graphics/MaterialLoader.h"
#include "../Utilities/Logger.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId MeshComponent::g_CompId = "MeshComponent";

MeshComponent::MeshComponent()
   : ActorComponent(), m_meshPath(), m_matPath(),
     m_pMesh(nullptr), m_pMaterial(nullptr)
{
}

bool MeshComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pMeshElement = pData->FirstChildElement("Mesh");
   if (pMeshElement) {
      const char* meshPath;
      pMeshElement->QueryStringAttribute("path", &meshPath);
      m_meshPath = meshPath;
      auto meshData = std::static_pointer_cast<MeshExtraData>(ResCache::Get()->GetHandle(m_meshPath)->GetExtra());

      if (meshData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading mesh for Actor with id: " + std::to_string(m_pOwner->GetId()));
         return false;
      }

      m_pMesh = meshData->GetMesh();
   }

   tinyxml2::XMLElement* pMatElement = pData->FirstChildElement("Material");
   if (pMatElement) {
      const char* matPath;
      pMatElement->QueryStringAttribute("path", &matPath);
      m_matPath = matPath;
      auto matData = std::static_pointer_cast<MaterialData>(ResCache::Get()->GetHandle(m_matPath)->GetExtra());

      if (matData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading mesh material for Actor with id: " + std::to_string(m_pOwner->GetId()));
         return false;
      }

      m_pMaterial = matData->GetMaterial();
   }

   return true;
}

void MeshComponent::OnRenderObject(const GameTimer& gt)
{
   RenderItemsStorage::OpaqueRenderItem opaqueRitem;
   opaqueRitem.actorId = m_pOwner->GetId();
   opaqueRitem.VAO = m_pMesh->GetVao();
   opaqueRitem.IndicesSize = m_pMesh->GetIndices().size();
   opaqueRitem.pMaterial = m_pMaterial;

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   opaqueRitem.ModelTransform = pTransformComponent->GetWorldTransformMatrix();

   g_pApp->TryGetHumanView(0)->GetScene()->GetRenderItemsStorage()->InsertOpaqueRenderItem(opaqueRitem);
}

tinyxml2::XMLElement* MeshComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pMeshElement = pDoc->NewElement("Mesh");
   pMeshElement->SetAttribute("path", m_meshPath.c_str());
   pBaseElement->LinkEndChild(pMeshElement);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("path", m_matPath.c_str());
   pBaseElement->LinkEndChild(pMaterialElement);

   return pBaseElement;
}

} // namespace BIEngine
