#include "NavWorld.h"

namespace BIEngine {

NavWorld::NavWorld()
   : m_pNavMeshManager(std::make_shared<NavMeshManager>()),
     m_pNavCrowd(std::make_shared<NavCrowd>())
{
}

bool NavWorld::Initialize()
{
   m_pNavMeshManager->LoadNavMesh("E:/BystrovI/Projects/BIEngine/Assets/Worlds/World/World.nav");

   if (!m_pNavCrowd->Initialize(m_pNavMeshManager)) {
      return false;
   }

   return true;
}

} // namespace BIEngine
