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
      fprintf(fp, "f %d %d %d\n", inds[i * 3], inds[i * 3 + 1], inds[i * 3 + 2]);
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

void NavMeshManager::RenderMesh()
{
   // Internal edges.
   const glm::vec3 coli = glm::vec3(0.0f, 0.0f, 1.0f);
   for (int i = 0; i < m_pNavMeshGenerator->GetDetailedMesh()->nmeshes; ++i) {
      const unsigned int* m = &m_pNavMeshGenerator->GetDetailedMesh()->meshes[i * 4];
      const unsigned int bverts = m[0];
      const unsigned int btris = m[2];
      const int ntris = (int)m[3];
      const float* verts = &m_pNavMeshGenerator->GetDetailedMesh()->verts[bverts * 3];
      const unsigned char* tris = &m_pNavMeshGenerator->GetDetailedMesh()->tris[btris * 4];

      for (int j = 0; j < ntris; ++j) {
         const unsigned char* t = &tris[j * 4];
         for (int k = 0, kp = 2; k < 3; kp = k++) {
            unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
            if (ef == 0) {
               // Internal edge
               if (t[kp] < t[k]) {
                  DebugDraw::Line(glm::vec3(verts[t[kp] * 3], verts[t[kp] * 3 + 1], verts[t[kp] * 3 + 2]), glm::vec3(verts[t[k] * 3], verts[t[k] * 3 + 1], verts[t[k] * 3 + 2]), coli);
               }
            }
         }
      }
   }

   // External edges.
   const glm::vec3 cole = glm::vec3(1.0f, 0.0f, 0.0f);
   for (int i = 0; i < m_pNavMeshGenerator->GetDetailedMesh()->nmeshes; ++i) {
      const unsigned int* m = &m_pNavMeshGenerator->GetDetailedMesh()->meshes[i * 4];
      const unsigned int bverts = m[0];
      const unsigned int btris = m[2];
      const int ntris = (int)m[3];
      const float* verts = &m_pNavMeshGenerator->GetDetailedMesh()->verts[bverts * 3];
      const unsigned char* tris = &m_pNavMeshGenerator->GetDetailedMesh()->tris[btris * 4];

      for (int j = 0; j < ntris; ++j) {
         const unsigned char* t = &tris[j * 4];
         for (int k = 0, kp = 2; k < 3; kp = k++) {
            unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
            if (ef != 0) {
               // Ext edge
               DebugDraw::Line(glm::vec3(verts[t[kp] * 3], verts[t[kp] * 3 + 1], verts[t[kp] * 3 + 2]), glm::vec3(verts[t[k] * 3], verts[t[k] * 3 + 1], verts[t[k] * 3 + 2]), cole);
            }
         }
      }
   }
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
