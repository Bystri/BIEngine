#pragma once

#include "NavMeshManager.h"

class dtCrowd;

namespace BIEngine {

typedef int NavAgentId;

class NavCrowd {
public:
   struct NavAgentParams {
      float Radius = 0.5f;
      float Height = 1.8f;
      float MaxAcceleration = 8.0f;
      float MaxSpeed = 3.5f;

      bool AnticipateTurns = false;

      bool OptimizeVis = false;
      float PathOptimizationRange = 15.0f;
      bool OptimizeTopo = false;

      float CollisionQueryRange = 6.0f;
      bool ObstacleAvoidance = false;

      bool Separation = false;
      float SeparationWeight = 5.0f;
   };

public:
   NavCrowd();
   ~NavCrowd();

   bool Initialize(std::shared_ptr<NavMeshManager> pNavMeshManager);

   NavAgentId AddAgent(std::shared_ptr<Actor> pActor, const NavAgentParams& params);
   void RemoveAgent(NavAgentId id);

   bool SetDestination(NavAgentId id, const glm::vec3& pos);
   glm::vec3 GetVelocity(NavAgentId id) const;

   void UpdateCrowdInfo(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap);
   void OnUpdate(const GameTimer& gt);

private:
   dtCrowd* m_pCrowd;
   std::shared_ptr<NavMeshManager> m_pNavMeshManager;
   std::unordered_map<ActorId, NavAgentId> m_actorToAgentMap;
   std::unordered_map<NavAgentId, ActorId> m_agentToActorMap;
};

} // namespace BIEngine
