#pragma once

#include "NavMeshManager.h"

class dtCrowd;

namespace BIEngine {

typedef int NavAgentId;

class NavCrowd {
public:
   NavCrowd();
   ~NavCrowd();

   bool Initialize(std::shared_ptr<NavMeshManager> pNavMeshManager);

   NavAgentId AddAgent(std::shared_ptr<Actor> pActor);
   void RemoveAgent(NavAgentId id);

   bool SetDestination(NavAgentId id, const glm::vec3& pos);
   glm::vec3 GetVelocity(NavAgentId id) const;

   void UpdateCrowdInfo(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap);
   void OnUpdate(const GameTimer& gt);

private:
   dtCrowd* m_pCrowd;
   std::shared_ptr<NavMeshManager> m_pNavMeshManager;
   std::unordered_map<ActorId, NavAgentId> m_actorToAgentMap;
};

} // namespace BIEngine
