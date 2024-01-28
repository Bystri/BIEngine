#pragma once

#include <memory>

class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
class rcContext;
class rcPolyMeshDetail;

namespace BIEngine {

class NavMeshInputGeometry;

class NavMeshGenerator {
public:
   enum class SamplePartitionType {
      WATERSHED,
      MONOTONE,
      LAYERS
   };

public:
   NavMeshGenerator();

   virtual ~NavMeshGenerator();

   NavMeshGenerator(const NavMeshGenerator&) = delete;
   NavMeshGenerator& operator=(const NavMeshGenerator&) = delete;

   virtual bool BuildNavmesh() = 0;

   void SetInputGeom(std::shared_ptr<NavMeshInputGeometry> pInputGeom);
   void ResetCommonSettings();

   const rcPolyMeshDetail* GetDetailedMesh() const { return m_pDmesh; }

protected:
   std::shared_ptr<NavMeshInputGeometry> m_pGeom;
   dtNavMesh* m_pNavMesh;
   dtNavMeshQuery* m_pNavQuery;
   dtCrowd* m_pCrowd;
   rcPolyMeshDetail* m_pDmesh;

   unsigned char m_navMeshDrawFlags;

   float m_cellSize;
   float m_cellHeight;
   float m_agentHeight;
   float m_agentRadius;
   float m_agentMaxClimb;
   float m_agentMaxSlope;
   float m_regionMinSize;
   float m_regionMergeSize;
   float m_edgeMaxLen;
   float m_edgeMaxError;
   float m_vertsPerPoly;
   float m_detailSampleDist;
   float m_detailSampleMaxError;
   SamplePartitionType m_partitionType;

   bool m_filterLowHangingObstacles;
   bool m_filterLedgeSpans;
   bool m_filterWalkableLowHeightSpans;

   rcContext* m_pCtx;
};

} // namespace BIEngine
