#include "NavMeshManager.h"

#include "../3rdParty/FastDelegate/FastDelegate.h"

#include "../Actors/RenderComponent.h"
#include "../Graphics/Model.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

NavMeshManager::NavMeshManager()
{
   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &NavMeshManager::HandleActorAdded), EvtData_Actor_Created::sk_EventType);
   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &NavMeshManager::HandleActorDestroyed), EvtData_Destroy_Actor::sk_EventType);
}

NavMeshManager::~NavMeshManager()
{
}

void NavMeshManager::BuildNavmesh()
{
   prepareNavGeom();
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

std::shared_ptr<NavInputMeshesManager> NavMeshManager::prepareNavGeom()
{
   std::vector<std::shared_ptr<Mesh>> meshes;

   for (int i = 0; i < m_actors.size(); ++i) {
      std::shared_ptr<BoxRenderComponent> pBoxRenderComponent = m_actors[i]->GetComponent<BoxRenderComponent>(BoxRenderComponent::g_CompId).lock();

      if (pBoxRenderComponent) {
         const std::vector<std::shared_ptr<ModelMesh>>& modelMeshes = pBoxRenderComponent->HackGetMeshes();

         for (int j = 0; j < modelMeshes.size(); ++j) {
            meshes.push_back(modelMeshes[j]->GetMesh());
         }
      }
   }

   return nullptr;
}

} // namespace BIEngine
