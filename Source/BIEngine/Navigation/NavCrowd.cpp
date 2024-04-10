#include "NavCrowd.h"

#include <DetourCrowd.h>

#include "../EngineCore/Assert.h"

namespace BIEngine {

static const int MAX_AGENTS = 128;
static const float MAX_HEIGHT = 1.0f;
static const float MAX_RADIUS = 5.0f;

NavCrowd::NavCrowd()
   : m_pCrowd(nullptr), m_pNavMeshManager(nullptr)
{
   m_pCrowd = dtAllocCrowd();
}

NavCrowd::~NavCrowd()
{
   dtFreeCrowd(m_pCrowd);
}

bool NavCrowd::Initialize(std::shared_ptr<NavMeshManager> pNavMeshManager)
{
   m_pNavMeshManager = pNavMeshManager;
   dtNavMesh* pNav = m_pNavMeshManager->GetNavMesh();

   m_pCrowd->init(MAX_AGENTS, MAX_RADIUS, pNav);

   // Setup local avoidance params to different qualities.
   dtObstacleAvoidanceParams params;
   // Use mostly default settings, copy from dtCrowd.
   std::memcpy(&params, m_pCrowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

   // Low (11)
   params.velBias = 0.5f;
   params.adaptiveDivs = 5;
   params.adaptiveRings = 2;
   params.adaptiveDepth = 1;
   m_pCrowd->setObstacleAvoidanceParams(0, &params);

   // Medium (22)
   params.velBias = 0.5f;
   params.adaptiveDivs = 5;
   params.adaptiveRings = 2;
   params.adaptiveDepth = 2;
   m_pCrowd->setObstacleAvoidanceParams(1, &params);

   // Good (45)
   params.velBias = 0.5f;
   params.adaptiveDivs = 7;
   params.adaptiveRings = 2;
   params.adaptiveDepth = 3;
   m_pCrowd->setObstacleAvoidanceParams(2, &params);

   // High (66)
   params.velBias = 0.5f;
   params.adaptiveDivs = 7;
   params.adaptiveRings = 3;
   params.adaptiveDepth = 3;

   m_pCrowd->setObstacleAvoidanceParams(3, &params);

   return true;
}

NavAgentId NavCrowd::AddAgent(std::shared_ptr<Actor> pActor, const NavAgentParams& params)
{
   std::shared_ptr<TransformComponent> pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   Assert(pTransformComponent != nullptr, "Actor has not TransformComponent. Something really bad happened");
   if (!pTransformComponent) {
      return -1;
   }

   dtCrowdAgentParams ap;
   memset(&ap, 0, sizeof(ap));
   ap.radius = params.Radius;
   ap.height = params.Height;
   ap.maxAcceleration = params.MaxAcceleration;
   ap.maxSpeed = params.MaxSpeed;
   ap.collisionQueryRange = params.CollisionQueryRange;
   ap.pathOptimizationRange = params.PathOptimizationRange;
   ap.updateFlags = 0;

   if (params.AnticipateTurns) {
      ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
   }
   if (params.OptimizeVis) {
      ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
   }
   if (params.OptimizeTopo) {
      ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
   }
   if (params.ObstacleAvoidance) {
      ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
   }
   if (params.Separation) {
      ap.updateFlags |= DT_CROWD_SEPARATION;
   }

   ap.obstacleAvoidanceType = 2;
   ap.separationWeight = params.SeparationWeight;

   NavAgentId agentId = m_pCrowd->addAgent(&pTransformComponent->GetPosition()[0], &ap);
   if (agentId == -1) {
      return -1;
   }

   m_actorToAgentMap[pActor->GetId()] = agentId;
   m_agentToActorMap[agentId] = pActor->GetId();

   return agentId;
}

void NavCrowd::RemoveAgent(NavAgentId id)
{
   m_pCrowd->removeAgent(id);

   ActorId actorId = m_agentToActorMap[id];

   m_actorToAgentMap.erase(actorId);
   m_agentToActorMap.erase(id);
}

bool NavCrowd::SetDestination(NavAgentId id, const glm::vec3& pos)
{
   const dtCrowdAgent* ag = m_pCrowd->getAgent(id);
   if (!ag->active) {
      return false;
   }

   dtNavMeshQuery* navquery = m_pNavMeshManager->GetNavMeshQuery();
   const dtQueryFilter* filter = m_pCrowd->getFilter(0);
   const float* halfExtents = m_pCrowd->getQueryExtents();

   float m_targetPos[3];
   dtPolyRef m_targetRef;

   navquery->findNearestPoly(&pos[0], halfExtents, filter, &m_targetRef, m_targetPos);

   return m_pCrowd->requestMoveTarget(id, m_targetRef, m_targetPos);
}

glm::vec3 NavCrowd::GetVelocity(NavAgentId id) const
{
   const dtCrowdAgent* ag = m_pCrowd->getAgent(id);
   if (!ag->active) {
      return glm::vec3(0.0f);
   }

   return glm::vec3(ag->vel[0], ag->vel[1], ag->vel[2]);
}

void NavCrowd::UpdateCrowdInfo(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap)
{
   for (auto itr = actorMap.cbegin(); itr != actorMap.end(); ++itr) {
      auto agentId = m_actorToAgentMap.find(itr->first);
      if (agentId != m_actorToAgentMap.end()) {
         dtCrowdAgent* ag = m_pCrowd->getEditableAgent(agentId->second);
         if (!ag->active) {
            continue;
         }

         std::shared_ptr<TransformComponent> pTransformComponent = itr->second->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
         Assert(pTransformComponent != nullptr, "Actor has not TransformComponent. Something really bad happened");
         if (!pTransformComponent) {
            continue;
         }

         const glm::vec3 pos = pTransformComponent->GetPosition();
         ag->npos[0] = pos.x;
         ag->npos[1] = pos.y;
         ag->npos[2] = pos.z;
      }
   }
}

void NavCrowd::OnUpdate(const GameTimer& gt)
{
   m_pCrowd->update(gt.DeltaTime(), nullptr);
}

} // namespace BIEngine
