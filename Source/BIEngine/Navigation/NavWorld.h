#pragma once

#include "NavCrowd.h"

namespace BIEngine {

class NavWorld {
public:
   NavWorld();

   bool Initialize();

   std::shared_ptr<NavMeshManager> GetNavMeshManager() { return m_pNavMeshManager; }

   std::shared_ptr<NavCrowd> GetNavCrowd() { return m_pNavCrowd; }

private:
   std::shared_ptr<NavMeshManager> m_pNavMeshManager;
   std::shared_ptr<NavCrowd> m_pNavCrowd;
};

} // namespace BIEngine
