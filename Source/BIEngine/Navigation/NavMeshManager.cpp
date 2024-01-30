#include "NavMeshManager.h"

#include "../3rdParty/FastDelegate/FastDelegate.h"

#include "NavMeshInputGeom.h"
#include "NavSoloMeshGenerator.h"
#include "../Actors/RenderComponent.h"
#include "../Graphics/Model.h"
#include "../EngineCore/GameApp.h"
#include "../Utilities/DebugDraw.h"

namespace BIEngine {

NavMeshManager::NavMeshManager()
{
   m_pNavMeshGenerator = std::make_shared<NavSoloMeshGenerator>();

   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &NavMeshManager::HandleActorAdded), EvtData_Actor_Created::sk_EventType);
   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &NavMeshManager::HandleActorDestroyed), EvtData_Destroy_Actor::sk_EventType);
}

NavMeshManager::~NavMeshManager()
{
}

static bool SaveGeom(const std::string& filepath, std::shared_ptr<NavMeshInputGeometry> pGeom)
{
   FILE* fp = fopen(filepath.c_str(), "w");
   if (!fp)
      return false;

   const float* verts = pGeom->GetMesh()->GetVerts();
   for (int i = 0; i < pGeom->GetMesh()->GetVertCount(); ++i) {
      fprintf(fp, "v %f %f %f\n", verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]);
   }

   const int* inds = pGeom->GetMesh()->GetTris();
   for (int i = 0; i < pGeom->GetMesh()->GetTriCount(); ++i) {
      fprintf(fp, "f %d %d %d\n", inds[i * 3] + 1, inds[i * 3 + 1] + 1, inds[i * 3 + 2] + 1);
   }

   fclose(fp);
}

void NavMeshManager::BuildNavmesh()
{
   std::shared_ptr<NavMeshInputGeometry> pGeom = prepareNavGeom();
   SaveGeom("geom.obj", pGeom);
   m_pNavMeshGenerator->SetInputGeom(pGeom);
   m_pNavMeshGenerator->BuildNavmesh();
}

static void duDebugDrawNavMeshPoly(const dtNavMesh& mesh, dtPolyRef ref, const glm::vec3& color)
{
   const dtMeshTile* tile = 0;
   const dtPoly* poly = 0;
   if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
      return;

   const unsigned int ip = (unsigned int)(poly - tile->polys);

   const dtPolyDetail* pd = &tile->detailMeshes[ip];

   std::vector<glm::vec3> polyVerts;

   constexpr float Y_LIFT = 0.5;

   for (int i = 0; i < pd->triCount; ++i) {
      const unsigned char* t = &tile->detailTris[(pd->triBase + i) * 4];
      for (int j = 0; j < 3; ++j) {
         if (t[j] < poly->vertCount)
            polyVerts.push_back(glm::vec3(tile->verts[poly->verts[t[j]] * 3], tile->verts[poly->verts[t[j]] * 3 + 1] + Y_LIFT, tile->verts[poly->verts[t[j]] * 3 + 2]));
         else
            polyVerts.push_back(glm::vec3(tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3], tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3 + 1] + Y_LIFT, tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3 + 2]));
      }
   }

   DebugDraw::Poly(polyVerts, color);

   for (int i = 0, j = polyVerts.size() - 1; i < polyVerts.size(); j = i++) {
      DebugDraw::Line(polyVerts[i], polyVerts[j], glm::vec3(0.0f, 0.0f, 1.0f));
   }
}

static void RenderNavMesh(const dtNavMesh& mesh)
{
   for (int i = 0; i < mesh.getMaxTiles(); ++i) {
      const dtMeshTile* const tile = mesh.getTile(i);
      if (!tile->header) {
         continue;
      }
      dtPolyRef base = mesh.getPolyRefBase(tile);

      for (int j = 0; j < tile->header->polyCount; ++j) {
         const dtPoly* p = &tile->polys[j];
         duDebugDrawNavMeshPoly(mesh, base | (dtPolyRef)j, glm::vec3(0.0f, 1.0f, 0.0f));
      }
   }
}

void NavMeshManager::RenderMesh()
{
   RenderNavMesh(*m_pNavMeshGenerator->GetNavMesh());
}

void NavMeshManager::HandleActorAdded(IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_Actor_Created> pCastEventData = std::static_pointer_cast<EvtData_Actor_Created>(pEventData);

   std::shared_ptr<Actor> pActor = g_pApp->m_pGameLogic->GetActor(pCastEventData->GetId());

   if (!pActor) {
      return;
   }

   TryAddActor(pActor);
}

void NavMeshManager::HandleActorDestroyed(IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
   TryRemoveActor(pCastEventData->GetId());
}

void NavMeshManager::TryAddActor(std::shared_ptr<Actor> pActor)
{
   std::shared_ptr<BoxRenderComponent> pBoxRenderComponent = pActor->GetComponent<BoxRenderComponent>(BoxRenderComponent::g_CompId).lock();

   if (pBoxRenderComponent) {
      m_actors.push_back(pActor);
   }
}

void NavMeshManager::TryRemoveActor(ActorId id)
{
   for (auto itr = m_actors.begin(); itr != m_actors.end(); ++itr) {
      if (itr->get()->GetId() == id) {
         m_actors.erase(itr);
         return;
      }
   }
}

std::shared_ptr<NavMeshInputGeometry> NavMeshManager::prepareNavGeom()
{
   std::shared_ptr<NavMeshInputGeometry> pGeom = std::make_shared<NavMeshInputGeometry>();

   std::shared_ptr<NavInputMeshesManager> pMeshesManager = std::make_shared<NavInputMeshesManager>();

   for (int i = 0; i < m_actors.size(); ++i) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_actors[i]->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
      std::shared_ptr<BoxRenderComponent> pBoxRenderComponent = m_actors[i]->GetComponent<BoxRenderComponent>(BoxRenderComponent::g_CompId).lock();

      if (pTransformComponent && pBoxRenderComponent) {
         const std::vector<std::shared_ptr<ModelMesh>>& modelMeshes = pBoxRenderComponent->HackGetMeshes();
         const glm::mat4 transformMatrix = pTransformComponent->GetTransformMatrix();

         for (int j = 0; j < modelMeshes.size(); ++j) {
            pMeshesManager->AddMesh(transformMatrix, modelMeshes[j]->GetMesh());
         }
      }
   }

   if (!pGeom->SetPreparedMesh(pMeshesManager)) {
      return nullptr;
   }

   return pGeom;
}

} // namespace BIEngine
