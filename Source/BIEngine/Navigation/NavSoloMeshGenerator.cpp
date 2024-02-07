#include "NavSoloMeshGenerator.h"

#include <DetourNavMeshBuilder.h>
#include <DetourNavMesh.h>

#include "NavMeshInputGeom.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

NavSoloMeshGenerator::NavSoloMeshGenerator()
   : NavMeshGenerator(),
     m_pTriareas(nullptr),
     m_pSolid(nullptr),
     m_pChf(nullptr),
     m_pCset(nullptr),
     m_pPolyMesh(nullptr),
     m_pDmesh(nullptr)
{
}

NavSoloMeshGenerator::~NavSoloMeshGenerator()
{
   cleanup();
}

void NavSoloMeshGenerator::cleanup()
{
   if (m_pTriareas) {
      delete[] m_pTriareas;
      m_pTriareas = nullptr;
   }

   if (m_pSolid) {
      rcFreeHeightField(m_pSolid);
      m_pSolid = nullptr;
   }

   if (m_pChf) {
      rcFreeCompactHeightfield(m_pChf);
      m_pChf = nullptr;
   }

   if (m_pCset) {
      rcFreeContourSet(m_pCset);
      m_pCset = nullptr;
   }

   if (m_pPolyMesh) {
      rcFreePolyMesh(m_pPolyMesh);
      m_pPolyMesh = nullptr;
   }

   if (m_pDmesh) {
      rcFreePolyMeshDetail(m_pDmesh);
      m_pDmesh = nullptr;
   }

   if (m_pNavMesh) {
      dtFreeNavMesh(m_pNavMesh);
      m_pNavMesh = nullptr;
   }
}

bool NavSoloMeshGenerator::BuildNavmesh()
{
   if (!m_pGeom || !m_pGeom->GetMesh()) {
      Logger::WriteErrorLog("buildNavigation: Input mesh is not specified.");
      return false;
   }

   cleanup();

   const float* bmin = m_pGeom->GetNavMeshBoundsMin();
   const float* bmax = m_pGeom->GetNavMeshBoundsMax();
   const float* verts = m_pGeom->GetMesh()->GetVerts();
   const int nverts = m_pGeom->GetMesh()->GetVertCount();
   const int* tris = m_pGeom->GetMesh()->GetTris();
   const int ntris = m_pGeom->GetMesh()->GetTriCount();


   initializeBuildConfig(bmin, bmax, nverts, ntris);

   // Reset build times gathering.
   m_pCtx->resetTimers();

   // Start the build process.
   m_pCtx->startTimer(RC_TIMER_TOTAL);

   Logger::WriteMsgLog("Building navigation:");
   Logger::WriteMsgLog(" - %d x %d cells", m_cfg.width, m_cfg.height);
   Logger::WriteMsgLog(" - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

   if (!rasterizeInputPolygons(verts, nverts, tris, ntris)) {
      return false;
   }

   filterWalkablesSurfaces();
   if (!partitionSurfaceToSimpleRegions()) {
      return false;
   }

   if (!traceAndSimplifyRegionContours()) {
      return false;
   }

   if (!buildPolygonMeshFromContours()) {
      return false;
   }

   if (!createDetailMesh()) {
      return false;
   }

   if (!createDetourData()) {
      return false;
   }

   m_pCtx->stopTimer(RC_TIMER_TOTAL);

   // Show performance stats.
   Logger::WriteMsgLog(">> Polymesh: %d vertices  %d polygons", m_pPolyMesh->nverts, m_pPolyMesh->npolys);

   return true;
}

void NavSoloMeshGenerator::initializeBuildConfig(const float* bmin, const float* bmax, const int nverts, const int ntris)
{
   // Init build configuration from GUI
   memset(&m_cfg, 0, sizeof(m_cfg));
   m_cfg.cs = m_cellSize;
   m_cfg.ch = m_cellHeight;
   m_cfg.walkableSlopeAngle = m_agentMaxSlope;
   m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
   m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
   m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
   m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
   m_cfg.maxSimplificationError = m_edgeMaxError;
   m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);     // Note: area = size*size
   m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize); // Note: area = size*size
   m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
   m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
   m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

   // Set the area where the navigation will be build.
   // Here the bounds of the input mesh are used, but the
   // area could be specified by an user defined box, etc.
   rcVcopy(m_cfg.bmin, bmin);
   rcVcopy(m_cfg.bmax, bmax);
   rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);
}

bool NavSoloMeshGenerator::rasterizeInputPolygons(const float* verts, const int nverts, const int* tris, const int ntris)
{
   // Allocate voxel heightfield where we rasterize our input data to.
   m_pSolid = rcAllocHeightfield();
   if (!m_pSolid) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'm_pSolid'");
      return false;
   }
   if (!rcCreateHeightfield(m_pCtx, *m_pSolid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch)) {
      Logger::WriteErrorLog("buildNavigation: Could not create solid heightfield");
      return false;
   }

   // Allocate array that can hold triangle area types.
   // If you have multiple meshes you need to process, allocate
   // and array which can hold the max number of triangles you need to process.
   m_pTriareas = new unsigned char[ntris];
   if (!m_pTriareas) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'm_triareas' (%d)", ntris);
      return false;
   }

   // Find triangles which are walkable based on their slope and rasterize them.
   // If your input data is multiple meshes, you can transform them here, calculate
   // the are type for each of the meshes and rasterize them.
   memset(m_pTriareas, 0, ntris * sizeof(unsigned char));
   rcMarkWalkableTriangles(m_pCtx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_pTriareas);
   if (!rcRasterizeTriangles(m_pCtx, verts, nverts, tris, m_pTriareas, ntris, *m_pSolid, m_cfg.walkableClimb)) {
      Logger::WriteErrorLog("buildNavigation: Could not rasterize triangles.");
      return false;
   }

   delete[] m_pTriareas;
   m_pTriareas = nullptr;

   return true;
}

void NavSoloMeshGenerator::filterWalkablesSurfaces()
{
   // Once all geoemtry is rasterized, we do initial pass of filtering to
   // remove unwanted overhangs caused by the conservative rasterization
   // as well as filter spans where the character cannot possibly stand.
   if (m_filterLowHangingObstacles) {
      rcFilterLowHangingWalkableObstacles(m_pCtx, m_cfg.walkableClimb, *m_pSolid);
   }
   if (m_filterLedgeSpans) {
      rcFilterLedgeSpans(m_pCtx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_pSolid);
   }
   if (m_filterWalkableLowHeightSpans) {
      rcFilterWalkableLowHeightSpans(m_pCtx, m_cfg.walkableHeight, *m_pSolid);
   }
}

bool NavSoloMeshGenerator::partitionSurfaceToSimpleRegions()
{
   // Compact the heightfield so that it is faster to handle from now on.
   // This will result more cache coherent data as well as the neighbours
   // between walkable cells will be calculated.
   m_pChf = rcAllocCompactHeightfield();
   if (!m_pChf) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'chf'");
      return false;
   }
   if (!rcBuildCompactHeightfield(m_pCtx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_pSolid, *m_pChf)) {
      Logger::WriteErrorLog("buildNavigation: Could not build compact data.");
      return false;
   }

   rcFreeHeightField(m_pSolid);
   m_pSolid = nullptr;

   // Erode the walkable area by agent radius.
   if (!rcErodeWalkableArea(m_pCtx, m_cfg.walkableRadius, *m_pChf)) {
      Logger::WriteErrorLog("buildNavigation: Could not erode.");
      return false;
   }

   // Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
   // There are 3 martitioning methods, each with some pros and cons:
   // 1) Watershed partitioning
   //   - the classic Recast partitioning
   //   - creates the nicest tessellation
   //   - usually slowest
   //   - partitions the heightfield into nice regions without holes or overlaps
   //   - the are some corner cases where this method creates produces holes and overlaps
   //      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
   //      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
   //   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
   // 2) Monotone partioning
   //   - fastest
   //   - partitions the heightfield into regions without holes and overlaps (guaranteed)
   //   - creates long thin polygons, which sometimes causes paths with detours
   //   * use this if you want fast navmesh generation
   // 3) Layer partitoining
   //   - quite fast
   //   - partitions the heighfield into non-overlapping regions
   //   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
   //   - produces better triangles than monotone partitioning
   //   - does not have the corner cases of watershed partitioning
   //   - can be slow and create a bit ugly tessellation (still better than monotone)
   //     if you have large open areas with small obstacles (not a problem if you use tiles)
   //   * good choice to use for tiled navmesh with medium and small sized tiles

   if (m_partitionType == SamplePartitionType::WATERSHED) {
      // Prepare for region partitioning, by calculating distance field along the walkable surface.
      if (!rcBuildDistanceField(m_pCtx, *m_pChf)) {
         Logger::WriteErrorLog("buildNavigation: Could not build distance field.");
         return false;
      }

      // Partition the walkable surface into simple regions without holes.
      if (!rcBuildRegions(m_pCtx, *m_pChf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
         Logger::WriteErrorLog("buildNavigation: Could not build watershed regions.");
         return false;
      }
   } else if (m_partitionType == SamplePartitionType::MONOTONE) {
      // Partition the walkable surface into simple regions without holes.
      // Monotone partitioning does not need distancefield.
      if (!rcBuildRegionsMonotone(m_pCtx, *m_pChf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
         Logger::WriteErrorLog("buildNavigation: Could not build monotone regions.");
         return false;
      }
   } else // SAMPLE_PARTITION_LAYERS
   {
      // Partition the walkable surface into simple regions without holes.
      if (!rcBuildLayerRegions(m_pCtx, *m_pChf, 0, m_cfg.minRegionArea)) {
         Logger::WriteErrorLog("buildNavigation: Could not build layer regions.");
         return false;
      }
   }

   return true;
}

bool NavSoloMeshGenerator::traceAndSimplifyRegionContours()
{
   // Create contours.
   m_pCset = rcAllocContourSet();
   if (!m_pCset) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'cset'.");
      return false;
   }
   if (!rcBuildContours(m_pCtx, *m_pChf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_pCset)) {
      Logger::WriteErrorLog("buildNavigation: Could not create contours.");
      return false;
   }

   return true;
}

bool NavSoloMeshGenerator::buildPolygonMeshFromContours()
{
   // Build polygon navmesh from the contours.
   m_pPolyMesh = rcAllocPolyMesh();
   if (!m_pPolyMesh) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'pmesh'.");
      return false;
   }
   if (!rcBuildPolyMesh(m_pCtx, *m_pCset, m_cfg.maxVertsPerPoly, *m_pPolyMesh)) {
      Logger::WriteErrorLog("buildNavigation: Could not triangulate contours.");
      return false;
   }

   return true;
}

bool NavSoloMeshGenerator::createDetailMesh()
{
   m_pDmesh = rcAllocPolyMeshDetail();
   if (!m_pDmesh) {
      Logger::WriteErrorLog("buildNavigation: Out of memory 'm_pDmesh'");
      return false;
   }

   if (!rcBuildPolyMeshDetail(m_pCtx, *m_pPolyMesh, *m_pChf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_pDmesh)) {
      Logger::WriteErrorLog("buildNavigation: Could not build detail mesh.");
      return false;
   }

   rcFreeCompactHeightfield(m_pChf);
   m_pChf = nullptr;
   rcFreeContourSet(m_pCset);
   m_pCset = nullptr;

   return true;
}

bool NavSoloMeshGenerator::createDetourData()
{
   if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
      unsigned char* navData = 0;
      int navDataSize = 0;

      for (int i = 0; i < m_pPolyMesh->npolys; ++i) {
         if (m_pPolyMesh->areas[i] == RC_WALKABLE_AREA) {
            m_pPolyMesh->areas[i] = 0;
            m_pPolyMesh->flags[i] = 1;
         }
      }

      dtNavMeshCreateParams params;
      memset(&params, 0, sizeof(params));
      params.verts = m_pPolyMesh->verts;
      params.vertCount = m_pPolyMesh->nverts;
      params.polys = m_pPolyMesh->polys;
      params.polyAreas = m_pPolyMesh->areas;
      params.polyFlags = m_pPolyMesh->flags;
      params.polyCount = m_pPolyMesh->npolys;
      params.nvp = m_pPolyMesh->nvp;
      params.detailMeshes = m_pDmesh->meshes;
      params.detailVerts = m_pDmesh->verts;
      params.detailVertsCount = m_pDmesh->nverts;
      params.detailTris = m_pDmesh->tris;
      params.detailTriCount = m_pDmesh->ntris;

      params.offMeshConCount = 0;

      params.walkableHeight = m_agentHeight;
      params.walkableRadius = m_agentRadius;
      params.walkableClimb = m_agentMaxClimb;
      rcVcopy(params.bmin, m_pPolyMesh->bmin);
      rcVcopy(params.bmax, m_pPolyMesh->bmax);
      params.cs = m_cfg.cs;
      params.ch = m_cfg.ch;
      params.buildBvTree = true;

      if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
         Logger::WriteErrorLog("Could not build Detour navmesh.");
         return false;
      }

      m_pNavMesh = dtAllocNavMesh();
      if (!m_pNavMesh) {
         dtFree(navData);
         Logger::WriteErrorLog("Could not create Detour navmesh");
         return false;
      }

      dtStatus status;

      status = m_pNavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
      if (dtStatusFailed(status)) {
         dtFree(navData);
         Logger::WriteErrorLog("Could not init Detour navmesh");
         return false;
      }

      return true;
   }
}

} // namespace BIEngine
