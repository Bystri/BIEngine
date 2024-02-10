#pragma once

#include <vector>
#include <memory>

#include <DetourNavMeshQuery.h>

#include "../EventManager/Events.h"

namespace BIEngine {

class Actgor;
class NavMeshInputGeometry;
class NavMeshGenerator;
class NavMeshBuildSettings;

class NavMeshManager {
public:
   NavMeshManager();
   ~NavMeshManager();

   void SaveNavMesh(const std::string& path);
   void LoadNavMesh(const std::string& path);
   bool BuildNavmesh();
   void RenderMesh();

   dtNavMesh* GetNavMesh();
   dtNavMeshQuery* GetNavMeshQuery();

private:
   void renderNavMeshWindow();

   bool generateNavmesh(const NavMeshBuildSettings& settings);

   void HandleActorAdded(IEventDataPtr pEventData);
   void HandleActorDestroyed(IEventDataPtr pEventData);

   void TryAddActor(std::shared_ptr<Actor> pActor);
   void TryRemoveActor(ActorId id);

   std::shared_ptr<NavMeshInputGeometry> prepareNavGeom();

private:
   std::vector<std::shared_ptr<Actor>> m_actors;
   std::shared_ptr<NavMeshGenerator> m_pNavMeshGenerator;
   dtNavMesh* m_pNavMesh;
   dtNavMeshQuery* m_pNavQuery;

   bool m_bRenderNavmesh;
};

} // namespace BIEngine
