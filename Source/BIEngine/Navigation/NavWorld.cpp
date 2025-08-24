#include "NavWorld.h"

namespace BIEngine {

NavWorld::NavWorld()
   : m_pNavMeshManager(MakeShared<NavMeshManager>()),
     m_pNavCrowd(MakeShared<NavCrowd>())
{
}

bool NavWorld::Initialize()
{
   m_pNavMeshManager->BuildNavmesh();

   if (!m_pNavCrowd->Initialize(m_pNavMeshManager)) {
      return false;
   }

   return true;
}

} // namespace BIEngine
