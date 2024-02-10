#pragma once

#include <Recast.h>

#include "NavMeshGenerator.h"

namespace BIEngine {

class NavSoloMeshGenerator : public NavMeshGenerator {
public:
   NavSoloMeshGenerator();
   virtual ~NavSoloMeshGenerator();

   virtual bool BuildNavmesh(const NavMeshBuildSettings& settings) override;

private:
   void cleanup();

   void initializeBuildConfig(const NavMeshBuildSettings& settings, const float* bmin, const float* bmax, const int nverts, const int ntris);
   bool rasterizeInputPolygons(const float* verts, const int nverts, const int* tris, const int ntris);
   void filterWalkablesSurfaces();
   bool partitionSurfaceToSimpleRegions(const NavMeshBuildSettings& settings);
   bool traceAndSimplifyRegionContours();
   bool buildPolygonMeshFromContours();
   bool createDetailMesh();
   bool createDetourData(const NavMeshBuildSettings& settings);

private:
   unsigned char* m_pTriareas;
   rcHeightfield* m_pSolid;
   rcCompactHeightfield* m_pChf;
   rcContourSet* m_pCset;
   rcPolyMesh* m_pPolyMesh;
   rcPolyMeshDetail* m_pDmesh;
   rcConfig m_cfg;
};

} // namespace BIEngine
