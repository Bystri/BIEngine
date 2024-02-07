#include "NavCrowd.h"

#include <DetourCrowd.h>

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

NavAgentId NavCrowd::AddAgent(std::shared_ptr<Actor> pActor)
{
   std::shared_ptr<TransformComponent> pTransformComponent = pActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   assert(pTransformComponent);
   if (!pTransformComponent) {
      return -1;
   }

   dtCrowdAgentParams ap;
   memset(&ap, 0, sizeof(ap));
   ap.radius = MAX_RADIUS;
   ap.height = MAX_HEIGHT;
   ap.maxAcceleration = 8.0f;
   ap.maxSpeed = 3.5f;
   ap.collisionQueryRange = ap.radius * 12.0f;
   ap.pathOptimizationRange = ap.radius * 30.0f;
   ap.updateFlags = 0;
   /*
   if (m_toolParams.m_anticipateTurns)
      ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
   if (m_toolParams.m_optimizeVis)
      ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
   if (m_toolParams.m_optimizeTopo)
      ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
   if (m_toolParams.m_obstacleAvoidance)
      ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
   if (m_toolParams.m_separation)
      ap.updateFlags |= DT_CROWD_SEPARATION;
   ap.obstacleAvoidanceType = (unsigned char)m_toolParams.m_obstacleAvoidanceType;
   ap.separationWeight = m_toolParams.m_separationWeight;
   */

   NavAgentId agentId = m_pCrowd->addAgent(&pTransformComponent->GetPosition()[0], &ap);
   if (agentId == -1) {
      return -1;
   }

   m_actorToAgentMap[pActor->GetId()] = agentId;

   return agentId;
}

void NavCrowd::RemoveAgent(NavAgentId id)
{
   m_pCrowd->removeAgent(id);
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
         assert(pTransformComponent);
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
