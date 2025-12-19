#pragma once

#include "NavMeshManager.h"

namespace BIEngine {

using NavAgentId = int;

class NavCrowd {
public:
   struct NavAgentParams {
      float MaxSpeed = 3.5f;
   };

private:
   struct NavAgent {
      enum class State : char {
         IDLE,
         WALKING
      };

      glm::vec3 pos;
      glm::vec3 curVel;
      glm::vec3 targetPos;
      float maxSpeed = 0.0f;
      bool isActive = false;
      State state = State::IDLE;
   };

public:
   bool Initialize(SharedPtr<NavMeshManager> pNavMeshManager);

   NavAgentId AddAgent(ActorId actorId, const glm::vec3& pos, const NavAgentParams& params);
   void RemoveAgent(NavAgentId id);

   bool SetDestination(NavAgentId id, const glm::vec3& pos);
   glm::vec3 GetVelocity(NavAgentId id) const;
   glm::vec3 GetDesiredInput(NavAgentId id) const;

   void UpdateCrowdInfo(const HashMap<ActorId, SharedPtr<Actor>>& actorMap);
   void OnUpdate(const GameTimer& gt);

private:
   HashMap<ActorId, NavAgentId> m_actorToAgentMap;
   HashMap<NavAgentId, ActorId> m_agentToActorMap;
   SharedPtr<NavMeshManager> m_pNavMeshManager;
   DynamicArray<NavAgent> m_navAgents;
};

} // namespace BIEngine
