#pragma once

#include <vector>
#include <memory>

#include "../EventManager/Events.h"

namespace BIEngine {

class Actgor;
class NavMeshInputGeometry;

class NavMeshManager {
public:
   NavMeshManager();
   ~NavMeshManager();

   void BuildNavmesh();

private:
   void HandleActorAdded(IEventDataPtr pEventData);
   void HandleActorDestroyed(IEventDataPtr pEventData);

   void TryAddActor(std::shared_ptr<Actor> pActor);
   void TryRemoveActor(ActorId id);

   std::shared_ptr<NavMeshInputGeometry> prepareNavGeom();

private:
   std::vector<std::shared_ptr<Actor>> m_actors;
};

} // namespace BIEngine
