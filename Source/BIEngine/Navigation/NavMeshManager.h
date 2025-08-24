#pragma once

#include <DetourNavMeshQuery.h>

#include "../EventManager/Events.h"
#include "../StdLib/SharedPtr.h"
#include "NavMeshGenerator.h"

namespace BIEngine {

class Actgor;
class NavMeshInputGeometry;
class NavMeshGenerator;

class NavMeshManager {
public:
   NavMeshManager();
   ~NavMeshManager();

   void SaveNavMesh(const String& path);
   void LoadNavMesh(const String& path);

   void SetBuildSettings(const NavMeshBuildSettings& settings) { m_buildSettings = settings; }

   bool BuildNavmesh();
   void DrawRenderDiagnostics();

   dtNavMesh* GetNavMesh();
   dtNavMeshQuery* GetNavMeshQuery();

private:
   void renderNavMeshWindow();

   bool generateNavmesh(const NavMeshBuildSettings& settings);

   void HandleActorAdded(IEventDataPtr pEventData);
   void HandleActorDestroyed(IEventDataPtr pEventData);

   void TryAddActor(SharedPtr<Actor> pActor);
   void TryRemoveActor(ActorId id);

   SharedPtr<NavMeshInputGeometry> prepareNavGeom();

private:
   EventManager::DelegateHandler m_handleActorAddedDelegateHandler;
   EventManager::DelegateHandler m_handleActorDestroyedDelegateHandler;

   NavMeshBuildSettings m_buildSettings;

   DynamicArray<SharedPtr<Actor>> m_actors;
   SharedPtr<NavMeshGenerator> m_pNavMeshGenerator;
   dtNavMesh* m_pNavMesh;
   dtNavMeshQuery* m_pNavQuery;

   bool m_bRenderNavmesh;
};

} // namespace BIEngine
