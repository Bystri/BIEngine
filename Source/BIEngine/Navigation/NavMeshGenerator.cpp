#include "NavMeshGenerator.h"

#include <Recast.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>

#include "NavMeshInputGeom.h"

namespace BIEngine {

NavMeshGenerator::NavMeshGenerator()
   : m_pGeom(nullptr),
     m_pNavMesh(nullptr),
     m_pNavQuery(nullptr),
     m_pCrowd(nullptr),
     m_filterLowHangingObstacles(true),
     m_filterLedgeSpans(true),
     m_filterWalkableLowHeightSpans(true),
     m_pCtx(new rcContext()),
     m_pDmesh(nullptr)
{
   ResetCommonSettings();
   m_pNavQuery = dtAllocNavMeshQuery();
   m_pCrowd = dtAllocCrowd();
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

   const NavBuildSettings* buildSettings = m_pGeom->GetBuildSettings();
   if (buildSettings) {
      m_cellSize = buildSettings->cellSize;
      m_cellHeight = buildSettings->cellHeight;
      m_agentHeight = buildSettings->agentHeight;
      m_agentRadius = buildSettings->agentRadius;
      m_agentMaxClimb = buildSettings->agentMaxClimb;
      m_agentMaxSlope = buildSettings->agentMaxSlope;
      m_regionMinSize = buildSettings->regionMinSize;
      m_regionMergeSize = buildSettings->regionMergeSize;
      m_edgeMaxLen = buildSettings->edgeMaxLen;
      m_edgeMaxError = buildSettings->edgeMaxError;
      m_vertsPerPoly = buildSettings->vertsPerPoly;
      m_detailSampleDist = buildSettings->detailSampleDist;
      m_detailSampleMaxError = buildSettings->detailSampleMaxError;
      m_partitionType = static_cast<SamplePartitionType>(buildSettings->partitionType);
   }
}

void NavMeshGenerator::ResetCommonSettings()
{
   m_cellSize = 0.3f;
   m_cellHeight = 0.2f;
   m_agentHeight = 2.0f;
   m_agentRadius = 0.6f;
   m_agentMaxClimb = 0.9f;
   m_agentMaxSlope = 45.0f;
   m_regionMinSize = 8;
   m_regionMergeSize = 20;
   m_edgeMaxLen = 12.0f;
   m_edgeMaxError = 1.3f;
   m_vertsPerPoly = 6.0f;
   m_detailSampleDist = 6.0f;
   m_detailSampleMaxError = 1.0f;
   m_partitionType = SamplePartitionType::WATERSHED;
}

} // namespace BIEngine
