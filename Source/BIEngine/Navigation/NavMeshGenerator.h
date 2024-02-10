#pragma once

#include <memory>

#include <DetourNavMeshQuery.h>

class dtNavMesh;
class dtCrowd;
class rcContext;
class rcPolyMeshDetail;

namespace BIEngine {

class NavMeshInputGeometry;

struct NavMeshBuildSettings {
   enum class SamplePartitionType {
      WATERSHED,
      MONOTONE,
      LAYERS
   };

   float CellSize = 0.3f;   // Cell size in world units
   float CellHeight = 0.2f; // Cell height in world units

   float AgentHeight = 2.0f;    // Agent height in world units
   float AgentRadius = 0.6;     // Agent radius in world units
   float AgentMaxClimb = 0.9;   // Agent max climb in world units
   float AgentMaxSlope = 45.0f; // Agent max slope in degrees


   float RegionMinSize = 8.0f;    // Region minimum size in voxels.
   float RegionMergeSize = 20.0f; // Region merge size in voxels.

   float EdgeMaxLen = 12.0f; // Edge max length in world units
   float EdgeMaxError = 1.3; // Edge max error in voxels
   float VertsPerPoly = 6.0f;

   float DetailSampleDist = 6.0f;     // Detail sample distance in voxels
   float DetailSampleMaxError = 1.0f; // Detail sample max error in voxel heights.

   SamplePartitionType PartitionType = SamplePartitionType::WATERSHED;

   bool FilterLowHangingObstacles = true;
   bool FilterLedgeSpans = true;
   bool FilterWalkableLowHeightSpans = true;
};

class NavMeshGenerator {
public:
   NavMeshGenerator();

   virtual ~NavMeshGenerator();

   NavMeshGenerator(const NavMeshGenerator&) = delete;
   NavMeshGenerator& operator=(const NavMeshGenerator&) = delete;

   virtual bool BuildNavmesh(const NavMeshBuildSettings& settings) = 0;

   dtNavMesh* GetNavMesh() { return m_pNavMesh; }

   void SetInputGeom(std::shared_ptr<NavMeshInputGeometry> pInputGeom);

protected:
   std::shared_ptr<NavMeshInputGeometry> m_pGeom;
   dtNavMesh* m_pNavMesh;

   unsigned char m_navMeshDrawFlags;

   bool m_filterLowHangingObstacles;
   bool m_filterLedgeSpans;
   bool m_filterWalkableLowHeightSpans;

   rcContext* m_pCtx;
};

} // namespace BIEngine
