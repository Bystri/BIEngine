#include "NavCrowd.h"

#include "../StdLib/Assert.h"
#include "../Actors/TransformComponent.h"

namespace BIEngine {

static const int MAX_AGENTS = 128;


struct NavSteeringBehaviourSeek {
   glm::vec3 charPos;
   glm::vec3 targetPos;

   float maxSpeed = 0.0f;

   glm::vec3 GetSteering()
   {
      glm::vec3 desiredVel = glm::normalize(targetPos - charPos);
      desiredVel *= maxSpeed;

      return desiredVel;
   }
};

struct NavSteeringBehaviourFlee {
   glm::vec3 charPos;
   glm::vec3 targetPos;

   float maxSpeed = 0.0f;

   glm::vec3 GetSteering()
   {
      glm::vec3 desiredVel = glm::normalize(charPos - targetPos);
      desiredVel *= maxSpeed;

      return desiredVel;
   }
};

struct NavSteeringBehaviourArrive {
   glm::vec3 charPos;
   glm::vec3 targetPos;

   float maxSpeed = 0.0f;

   float targetRadius = 1.0f;
   float slowdownRadius = 2.0f;

   glm::vec3 GetSteering()
   {
      const glm::vec3 vecToTarget = targetPos - charPos;
      const float distToTarget = glm::length(vecToTarget);

      if (distToTarget < targetRadius) {
         return glm::vec3(0.0f);
      }

      float targetSpeed = 0.0f;
      if (distToTarget > slowdownRadius) {
         targetSpeed = maxSpeed;
      } else {
         targetSpeed = maxSpeed * distToTarget / slowdownRadius;
      }

      if (targetSpeed > maxSpeed) {
         targetSpeed = maxSpeed;
      }

      return glm::normalize(vecToTarget) * targetSpeed;
   }
};

bool NavCrowd::Initialize(SharedPtr<NavMeshManager> pNavMeshManager)
{
   m_pNavMeshManager = pNavMeshManager;
   m_navAgents.Resize(MAX_AGENTS);

   return true;
}

NavAgentId NavCrowd::AddAgent(ActorId actorId, const glm::vec3& pos, const NavAgentParams& params)
{
   NavAgentId agentId = -1;
   for (int i = 0; i < m_navAgents.Size(); ++i) {
      if (!m_navAgents[i].isActive) {
         agentId = i;
         break;
      }
   }

   if (agentId == -1) {
      Logger::WriteErrorLog("Max number of available nav agents is reached. Cur max is %d", MAX_AGENTS);
      return -1;
   }

   m_navAgents[agentId] = NavAgent();
   m_navAgents[agentId].isActive = true;
   m_navAgents[agentId].maxSpeed = params.MaxSpeed;
   m_navAgents[agentId].pos = pos;
   m_navAgents[agentId].state = NavAgent::State::IDLE;

   m_actorToAgentMap[actorId] = agentId;
   m_agentToActorMap[agentId] = actorId;

   return agentId;
}

void NavCrowd::RemoveAgent(NavAgentId id)
{
   m_navAgents[id].isActive = false;

   ActorId actorId = m_agentToActorMap[id];

   m_actorToAgentMap.Erase(actorId);
   m_agentToActorMap.Erase(id);
}

bool NavCrowd::SetDestination(NavAgentId id, const glm::vec3& pos)
{
   NavAgent* const ag = &m_navAgents[id];
   if (!ag->isActive) {
      return false;
   }

   ag->state = NavAgent::State::WALKING;
   ag->targetPos = pos;

   return true;
}

glm::vec3 NavCrowd::GetVelocity(NavAgentId id) const
{
   if (!m_navAgents[id].isActive || m_navAgents[id].state != NavAgent::State::WALKING) {
      return glm::vec3(0.0f);
   }

   NavSteeringBehaviourArrive arrive;
   arrive.charPos = m_navAgents[id].pos;
   arrive.targetPos = m_navAgents[id].targetPos;
   arrive.maxSpeed = m_navAgents[id].maxSpeed;

   return arrive.GetSteering();
}

glm::vec3 NavCrowd::GetDesiredInput(NavAgentId id) const
{
   const NavAgent* const ag = &m_navAgents[id];
   if (!ag->isActive || ag->maxSpeed < std::numeric_limits<decltype(ag->maxSpeed)>::epsilon()) {
      return glm::vec3(0.0f);
   }

   return GetVelocity(id) / ag->maxSpeed;
}

void NavCrowd::UpdateCrowdInfo(const HashMap<ActorId, SharedPtr<Actor>>& actorMap)
{
   for (auto itr = actorMap.CBegin(); itr != actorMap.CEnd(); ++itr) {
      auto agentId = m_actorToAgentMap.Find(itr->first);
      if (agentId != m_actorToAgentMap.End()) {
         NavAgent* const ag = &m_navAgents[agentId->second];
         if (!ag->isActive) {
            continue;
         }

         SharedPtr<TransformComponent> pTransformComponent = itr->second->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
         Assert(pTransformComponent != nullptr, "Actor has not TransformComponent. Something really bad happened");
         if (!pTransformComponent) {
            continue;
         }

         ag->pos = pTransformComponent->GetPosition();
      }
   }
}

void NavCrowd::OnUpdate(const GameTimer& gt)
{
}

} // namespace BIEngine
