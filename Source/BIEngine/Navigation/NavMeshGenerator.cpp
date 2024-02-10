#include "NavMeshGenerator.h"

#include <Recast.h>

#include "NavMeshInputGeom.h"

namespace BIEngine {

NavMeshGenerator::NavMeshGenerator()
   : m_pGeom(nullptr),
     m_pNavMesh(nullptr),
     m_filterLowHangingObstacles(true),
     m_filterLedgeSpans(true),
     m_filterWalkableLowHeightSpans(true),
     m_pCtx(new rcContext())
{
}

NavMeshGenerator::~NavMeshGenerator()
{
   if (m_pCtx) {
      delete m_pCtx;
      m_pCtx = nullptr;
   }
}

void NavMeshGenerator::SetInputGeom(std::shared_ptr<NavMeshInputGeometry> pInputGeom)
{
   m_pGeom = pInputGeom;
}

} // namespace BIEngine
