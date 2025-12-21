#include "AiCrowdTargeterRecast.h"

#include <DetourPathCorridor.h>
#include <DetourPathQueue.h>

namespace BIEngine {

/// The maximum number of corners a crowd agent will look ahead in the path.
/// This value is used for sizing the crowd agent corner buffers.
/// Due to the behavior of the crowd manager, the actual number of useful
/// corners will be one less than this number.
static const int AI_CROWD_MAX_CORNERS = 4;

static constexpr int AI_CROWD_MAX_PATH_RESULTS = 256;
static constexpr int AI_CROWD_MAX_COMMON_NODES = 512;

class AiCrowdTargeterRecast : public AiCrowdTargeter {
private:
   enum class TargetState {
      NONE,
      FAILED,
      VALID,
      REQUESTING,
      WAITING_FOR_QUEUE,
      WAITING_FOR_PATH
   };

   struct AgentContext {
      dtPathCorridor corridor;                         /// The path corridor the agent is using.
      dtPolyRef targetRef;                             ///< Target polyref of the movement request.
      dtPathQueueRef targetPathqRef;                   ///< Path finder ref.
      Vector3 fromPos;                                 ///< Agent position right after movement request was made.
      Vector3 targetPos;                               ///< Target position of the movement request.
      float targetReplanTime = 0.0f;                   /// <Time since the agent's target was replanned.
      int cornersNum = 0;                              ///< Number of corners in the path
      float cornerVerts[AI_CROWD_MAX_CORNERS * 3];     ///< The local path corridor corners for the agent. (Staight path.) [(x, y, z) * #ncorners]
      dtPolyRef cornerPolys[AI_CROWD_MAX_CORNERS];     ///< The reference id of the polygon being entered at the corner. [(polyRef) * #ncorners]
      unsigned char cornerFlags[AI_CROWD_MAX_CORNERS]; ///< The local path corridor corner flags. (See: #dtStraightPathFlags) [(flags) * #ncorners]
      TargetState targetState = TargetState::NONE;     ///< State of the movement request.
      bool targetReplan = false;                       ///< Flag indicating that the current path is being replanned.
      bool partial = false;                            /// True if the agent has valid path (targetState == DT_CROWDAGENT_TARGET_VALID) and the path does not lead to the requested position, else false.
   };

public:
   virtual bool Init(const int maxAgents, SharedPtr<NavMeshManager> pNavMeshManager) override;

   virtual bool SetDestination(const int idx, const Vector3& toPos) override;
   virtual bool ResetDestination(const int idx) override;

   virtual Vector3 GetTarget(const int idx) const override;

   virtual void UpdateAgentPos(const int idx, const Vector3& pos) override;
   virtual void Update(const GameTimer& gt) override;

private:
   bool requestMoveTarget(const int idx, dtPolyRef ref, const Vector3& pos);
   bool requestMoveTargetReplan(const int idx, dtPolyRef ref, const Vector3& pos);

   int addToPathQueue(AgentContext* newag, AgentContext** agents, const int nagents, const int maxAgents);

   void checkPathValidity(const float dt);
   void updateMoveRequest();
   void findNextCornerToSteerTo();

private:
   dtPathQueue m_pathQueue;
   dtQueryFilter m_dummyFilter;

   DynamicArray<AgentContext> m_agents;

   Vector3 m_agentPlacementHalfExtents;

   SharedPtr<NavMeshManager> m_pNavMeshManager;
   dtNavMeshQuery* m_pNavQuery;
   dtPolyRef* m_pPathResult;
};

static void aiCrowdFloatsToVec(BIEngine::Vector3& toVec, float* fromVec)
{
   toVec.x = fromVec[0];
   toVec.y = fromVec[1];
   toVec.z = fromVec[2];
}

static void aiCrowdVecToFloats(float* toVec, const BIEngine::Vector3& fromVec)
{
   toVec[0] = fromVec.x;
   toVec[1] = fromVec.y;
   toVec[2] = fromVec.z;
}

bool AiCrowdTargeterRecast::Init(const int maxAgents, SharedPtr<NavMeshManager> pNavMeshManager)
{
   m_pNavMeshManager = pNavMeshManager;

   m_agentPlacementHalfExtents = {1.0f, 1.0f, 1.0f};

   m_agents.Resize(maxAgents);

   for (int i = 0; i < maxAgents; ++i) {
      if (!m_agents[i].corridor.init(AI_CROWD_MAX_PATH_RESULTS)) {
         return false;
      }
   }

   m_pNavQuery = dtAllocNavMeshQuery();
   if (!m_pNavQuery) {
      return false;
   }

   if (dtStatusFailed(m_pNavQuery->init(pNavMeshManager->GetNavMesh(), AI_CROWD_MAX_COMMON_NODES))) {
      return false;
   }

   return true;
}

bool AiCrowdTargeterRecast::SetDestination(const int idx, const Vector3& toPos)
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return false;
   }

   float targetPos[3];
   dtPolyRef targetRef;

   m_pNavQuery->findNearestPoly(toPos.data.Data(), m_agentPlacementHalfExtents.data.Data(), &m_dummyFilter, &targetRef, targetPos);

   return requestMoveTarget(idx, targetRef, Vector3(targetPos[0], targetPos[1], targetPos[2]));
}

bool AiCrowdTargeterRecast::ResetDestination(const int idx)
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return false;
   }

   AgentContext& ag = m_agents[idx];

   // Initialize request.
   ag.targetRef = 0;
   ag.targetPos = Vector3(0.0f);
   ag.targetPathqRef = DT_PATHQ_INVALID;
   ag.targetReplan = false;
   ag.targetState = TargetState::NONE;

   return true;
}

Vector3 AiCrowdTargeterRecast::GetTarget(const int idx) const
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return Vector3(0.0f);
   }

   const AgentContext& ag = m_agents[idx];

   if (ag.targetState == TargetState::NONE) {
      return Vector3(0.0f);
   }

   const float* target = &ag.cornerVerts[Min(1, ag.cornersNum - 1) * 3];

   return Vector3(target[0], target[1], target[2]);
}

void AiCrowdTargeterRecast::UpdateAgentPos(const int idx, const Vector3& pos)
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return;
   }

   m_agents[idx].fromPos = pos;
}

void AiCrowdTargeterRecast::Update(const GameTimer& gt)
{
   checkPathValidity(gt.DeltaTime());
   updateMoveRequest();
   findNextCornerToSteerTo();
}

int AiCrowdTargeterRecast::addToPathQueue(AgentContext* newag, AgentContext** agents, const int nagents, const int maxAgents)
{
   // Insert neighbour based on greatest time.
   int slot = 0;
   if (!nagents) {
      slot = nagents;
   } else if (newag->targetReplanTime <= agents[nagents - 1]->targetReplanTime) {
      if (nagents >= maxAgents) {
         return nagents;
      }
      slot = nagents;
   } else {
      int i;
      for (i = 0; i < nagents; ++i) {
         if (newag->targetReplanTime >= agents[i]->targetReplanTime) {
            break;
         }
      }

      const int toAddIdx = i + 1;
      const int n = BIEngine::Min(nagents - i, maxAgents - toAddIdx);

      BIEngine::Assert(toAddIdx + n <= maxAgents, "Execeed maximum number of agents");

      if (n > 0) {
         memmove(&agents[toAddIdx], &agents[i], sizeof(AgentContext*) * n);
      }
      slot = i;
   }

   agents[slot] = newag;

   return BIEngine::Min(nagents + 1, maxAgents);
}

bool AiCrowdTargeterRecast::requestMoveTarget(const int idx, dtPolyRef ref, const BIEngine::Vector3& toPos)
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return false;
   }

   if (!ref) {
      return false;
   }

   AgentContext& ag = m_agents[idx];

   // Initialize request.
   ag.targetRef = ref;
   ag.targetPos = toPos;
   ag.targetPathqRef = DT_PATHQ_INVALID;
   ag.targetReplan = false;
   if (ag.targetRef) {
      ag.targetState = TargetState::REQUESTING;
   } else {
      ag.targetState = TargetState::FAILED;
   }

   return true;
}

bool AiCrowdTargeterRecast::requestMoveTargetReplan(const int idx, dtPolyRef ref, const Vector3& toPos)
{
   if (idx < 0 || idx >= m_agents.Size()) {
      return false;
   }

   if (!ref) {
      return false;
   }

   AgentContext& ag = m_agents[idx];

   // Initialize request.
   ag.targetRef = ref;
   ag.targetPos = toPos;
   ag.targetPathqRef = DT_PATHQ_INVALID;
   ag.targetReplan = true;
   if (ag.targetRef) {
      ag.targetState = TargetState::REQUESTING;
   } else {
      ag.targetState = TargetState::FAILED;
   }

   return true;
}

void AiCrowdTargeterRecast::checkPathValidity(const float dt)
{
   static const int CHECK_LOOKAHEAD = 10;
   static const float TARGET_REPLAN_DELAY = 1.0; // seconds

   for (int i = 0; i < m_agents.Size(); ++i) {
      AgentContext& ag = m_agents[i];

      if (ag.targetState == TargetState::NONE) {
         continue;
      }

      ag.targetReplanTime += dt;

      bool replan = false;

      float agentPos[3];
      dtPolyRef agentRef = ag.corridor.getFirstPoly();
      aiCrowdVecToFloats(agentPos, ag.fromPos);
      if (!m_pNavQuery->isValidPolyRef(agentRef, &m_dummyFilter)) {
         // Current location is not valid, try to reposition.
         // TODO: this can snap agents, how to handle that?
         float nearest[3];
         aiCrowdVecToFloats(nearest, ag.fromPos);
         agentRef = 0;
         m_pNavQuery->findNearestPoly(ag.fromPos.data.Data(), m_agentPlacementHalfExtents.data.Data(), &m_dummyFilter, &agentRef, nearest);

         agentPos[0] = nearest[0];
         agentPos[1] = nearest[1];
         agentPos[2] = nearest[2];

         if (!agentRef) {
            // Could not find location in navmesh, set state to invalid.
            ag.corridor.reset(0, agentPos);
            ag.partial = false;
            ag.targetState = TargetState::FAILED;
            continue;
         }

         // Make sure the first polygon is valid, but leave other valid
         // polygons in the path so that replanner can adjust the path better.

         if (ag.corridor.getPathCount() == 0) {
            ag.corridor.reset(agentRef, agentPos);
         } else {
            ag.corridor.fixPathStart(agentRef, agentPos);
         }

         //			ag->corridor.trimInvalidPath(agentRef, agentPos, m_navquery, &m_filter);
         aiCrowdFloatsToVec(ag.fromPos, agentPos);

         replan = true;
      }

      // If the agent does not have move target or is controlled by velocity, no need to recover the target nor replan.
      if (ag.targetState == TargetState::NONE) {
         continue;
      }

      // Try to recover move request position.
      if (ag.targetState != TargetState::NONE && ag.targetState != TargetState::FAILED) {
         if (!m_pNavQuery->isValidPolyRef(ag.targetRef, &m_dummyFilter)) {
            // Current target is not valid, try to reposition.
            float nearest[3];
            aiCrowdVecToFloats(nearest, ag.targetPos);
            ag.targetRef = 0;
            m_pNavQuery->findNearestPoly(ag.targetPos.data.Data(), m_agentPlacementHalfExtents.data.Data(), &m_dummyFilter, &ag.targetRef, nearest);
            aiCrowdFloatsToVec(ag.targetPos, nearest);
            replan = true;
         }
         if (!ag.targetRef) {
            // Failed to reposition target, fail moverequest.
            ag.corridor.reset(agentRef, agentPos);
            ag.partial = false;
            ag.targetState = TargetState::NONE;
         }
      }

      // If nearby corridor is not valid, replan.
      if (!ag.corridor.isValid(CHECK_LOOKAHEAD, m_pNavQuery, &m_dummyFilter)) {
         // Fix current path.
         //			ag->corridor.trimInvalidPath(agentRef, agentPos, m_navquery, &m_filter);
         replan = true;
      }

      // If the end of the path is near and it is not the requested location, replan.
      if (ag.targetState == TargetState::VALID) {
         if (ag.targetReplanTime > TARGET_REPLAN_DELAY && ag.corridor.getPathCount() < CHECK_LOOKAHEAD && ag.corridor.getLastPoly() != ag.targetRef) {
            replan = true;
         }
      }

      // Try to replan path to goal.
      if (replan) {
         if (ag.targetState != TargetState::NONE) {
            requestMoveTargetReplan(i, ag.targetRef, ag.targetPos);
         }
      }
   }
}

void AiCrowdTargeterRecast::updateMoveRequest()
{
   const int PATH_MAX_AGENTS = 8;
   AgentContext* queue[PATH_MAX_AGENTS];
   int nqueue = 0;

   // Fire off new requests.
   for (int i = 0; i < m_agents.Size(); ++i) {
      AgentContext& ag = m_agents[i];

      if (ag.targetState == TargetState::NONE) {
         continue;
      }

      if (ag.targetState == TargetState::REQUESTING) {
         const dtPolyRef* path = ag.corridor.getPath();
         const int npath = ag.corridor.getPathCount();
         BIEngine::Assert(npath, "Corridor path for agent with idx %d doesn't reseted after init", i);

         static const int MAX_RES = 32;
         float reqPos[3];
         dtPolyRef reqPath[MAX_RES]; // The path to the request location
         int reqPathCount = 0;

         // Quick search towards the goal.
         static const int MAX_ITER = 20;
         m_pNavQuery->initSlicedFindPath(path[0], ag.targetRef, ag.fromPos.data.Data(), ag.targetPos.data.Data(), &m_dummyFilter);
         m_pNavQuery->updateSlicedFindPath(MAX_ITER, 0);
         dtStatus status = 0;
         if (ag.targetReplan) {
            // Try to use existing steady path during replan if possible.
            status = m_pNavQuery->finalizeSlicedFindPathPartial(path, npath, reqPath, &reqPathCount, MAX_RES);
         } else {
            // Try to move towards target when goal changes.
            status = m_pNavQuery->finalizeSlicedFindPath(reqPath, &reqPathCount, MAX_RES);
         }

         if (!dtStatusFailed(status) && reqPathCount > 0) {
            // In progress or succeed.
            if (reqPath[reqPathCount - 1] != ag.targetRef) {
               // Partial path, constrain target position inside the last polygon.
               status = m_pNavQuery->closestPointOnPoly(reqPath[reqPathCount - 1], ag.targetPos.data.Data(), reqPos, 0);
               if (dtStatusFailed(status)) {
                  reqPathCount = 0;
               }
            } else {
               aiCrowdVecToFloats(reqPos, ag.targetPos);
            }
         } else {
            reqPathCount = 0;
         }

         if (!reqPathCount) {
            // Could not find path, start the request from current location.
            aiCrowdVecToFloats(reqPos, ag.fromPos);
            reqPath[0] = path[0];
            reqPathCount = 1;
         }

         ag.corridor.setCorridor(reqPos, reqPath, reqPathCount);
         ag.partial = false;

         if (reqPath[reqPathCount - 1] == ag.targetRef) {
            ag.targetState = TargetState::VALID;
            ag.targetReplanTime = 0.0;
         } else {
            // The path is longer or potentially unreachable, full plan.
            ag.targetState = TargetState::WAITING_FOR_QUEUE;
         }
      }

      if (ag.targetState == TargetState::WAITING_FOR_QUEUE) {
         nqueue = addToPathQueue(&ag, queue, nqueue, PATH_MAX_AGENTS);
      }
   }

   for (int i = 0; i < nqueue; ++i) {
      AgentContext* ag = queue[i];
      ag->targetPathqRef = m_pathQueue.request(ag->corridor.getLastPoly(), ag->targetRef, ag->corridor.getTarget(), ag->targetPos.data.Data(), &m_dummyFilter);
      if (ag->targetPathqRef != DT_PATHQ_INVALID) {
         ag->targetState = TargetState::WAITING_FOR_PATH;
      }
   }


   // Update requests.
   constexpr int MAX_ITERS_PER_UPDATE = 100;
   m_pathQueue.update(MAX_ITERS_PER_UPDATE);

   dtStatus status;

   // Process path results.
   for (int i = 0; i < m_agents.Size(); ++i) {
      AgentContext& ag = m_agents[i];

      if (ag.targetState == TargetState::NONE) {
         continue;
      }

      if (ag.targetState == TargetState::WAITING_FOR_PATH) {
         // Poll path queue.
         status = m_pathQueue.getRequestStatus(ag.targetPathqRef);
         if (dtStatusFailed(status)) {
            // Path find failed, retry if the target location is still valid.
            ag.targetPathqRef = DT_PATHQ_INVALID;
            if (ag.targetRef) {
               ag.targetState = TargetState::REQUESTING;
            } else {
               ag.targetState = TargetState::FAILED;
            }
            ag.targetReplanTime = 0.0;
         } else if (dtStatusSucceed(status)) {
            const dtPolyRef* path = ag.corridor.getPath();
            const int npath = ag.corridor.getPathCount();
            BIEngine::Assert(npath, "Corridor path for agent with idx %d doesn't reseted after init", i);

            // Apply results.
            BIEngine::Vector3 targetPos;
            targetPos = ag.targetPos;

            dtPolyRef* res = m_pPathResult;
            bool valid = true;
            int nres = 0;
            status = m_pathQueue.getPathResult(ag.targetPathqRef, res, &nres, AI_CROWD_MAX_PATH_RESULTS);
            if (dtStatusFailed(status) || !nres)
               valid = false;

            if (dtStatusDetail(status, DT_PARTIAL_RESULT)) {
               ag.partial = true;
            } else {
               ag.partial = false;
            }

            // Merge result and existing path.
            // The agent might have moved whilst the request is
            // being processed, so the path may have changed.
            // We assume that the end of the path is at the same location
            // where the request was issued.

            // The last ref in the old path should be the same as
            // the location where the request was issued..
            if (valid && path[npath - 1] != res[0])
               valid = false;

            if (valid) {
               // Put the old path infront of the old path.
               if (npath > 1) {
                  // Make space for the old path.
                  if ((npath - 1) + nres > AI_CROWD_MAX_PATH_RESULTS)
                     nres = AI_CROWD_MAX_PATH_RESULTS - (npath - 1);

                  memmove(res + npath - 1, res, sizeof(dtPolyRef) * nres);
                  // Copy old path in the beginning.
                  memcpy(res, path, sizeof(dtPolyRef) * (npath - 1));
                  nres += npath - 1;

                  // Remove trackbacks
                  for (int j = 0; j < nres; ++j) {
                     if (j - 1 >= 0 && j + 1 < nres) {
                        if (res[j - 1] == res[j + 1]) {
                           memmove(res + (j - 1), res + (j + 1), sizeof(dtPolyRef) * (nres - (j + 1)));
                           nres -= 2;
                           j -= 2;
                        }
                     }
                  }
               }

               // Check for partial path.
               if (res[nres - 1] != ag.targetRef) {
                  // Partial path, constrain target position inside the last polygon.
                  float nearest[3];
                  status = m_pNavQuery->closestPointOnPoly(res[nres - 1], targetPos.data.Data(), nearest, 0);
                  if (dtStatusSucceed(status)) {
                     aiCrowdFloatsToVec(targetPos, nearest);
                  } else {
                     valid = false;
                  }
               }
            }

            if (valid) {
               // Set current corridor.
               ag.corridor.setCorridor(targetPos.data.Data(), res, nres);
               ag.targetState = TargetState::VALID;
            } else {
               // Something went wrong.
               ag.targetState = TargetState::FAILED;
            }

            ag.targetReplanTime = 0.0;
         }
      }
   }
}

void AiCrowdTargeterRecast::findNextCornerToSteerTo()
{
   for (int i = 0; i < m_agents.Size(); ++i) {
      AgentContext& ag = m_agents[i];

      if (ag.targetState == TargetState::NONE) {
         continue;
      }

      // Find corners for steering
      ag.cornersNum = ag.corridor.findCorners(ag.cornerVerts, ag.cornerFlags, ag.cornerPolys, AI_CROWD_MAX_CORNERS, m_pNavQuery, &m_dummyFilter);
   }
}

SharedPtr<AiCrowdTargeter> CreateAiCrowdTargeterRecast()
{
   return MakeShared<AiCrowdTargeterRecast>();
}

} // namespace BIEngine
