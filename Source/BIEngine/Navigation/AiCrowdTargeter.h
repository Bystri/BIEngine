#pragma once

#include "../../BIEngine/Math/Vector.h"
#include "../../BIEngine/Navigation/NavMeshManager.h"

namespace BIEngine {

class AiCrowdTargeter {
public:
   virtual ~AiCrowdTargeter() = default;

   virtual bool Init(int maxAgents, SharedPtr<NavMeshManager> pNavMeshManager) = 0;

   virtual bool SetDestination(int agentId, const Vector3& toPos) = 0;
   virtual bool ResetDestination(const int idx) = 0;

   virtual Vector3 GetTarget(const int idx) const = 0;

   virtual void UpdateAgentPos(const int idx, const Vector3& pos) = 0;
   virtual void Update(const GameTimer& gt) = 0;

#ifndef _RETAIL
   virtual void DrawDebug() {};
#endif

};

} // namespace BIEngine
