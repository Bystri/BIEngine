#pragma once

#include "NavCrowd.h"

namespace BIEngine {

class NavWorld {
public:
   NavWorld();

   bool Initialize();

   SharedPtr<NavMeshManager> GetNavMeshManager() { return m_pNavMeshManager; }

   SharedPtr<NavCrowd> GetNavCrowd() { return m_pNavCrowd; }

private:
   SharedPtr<NavMeshManager> m_pNavMeshManager;
   SharedPtr<NavCrowd> m_pNavCrowd;
};

} // namespace BIEngine
