#pragma once

#include <glm/glm.hpp>

#include "NavChunkyTriMesh.h"
#include "NavInputMeshesManager.h"
#include "../Renderer/Mesh.h"

namespace BIEngine {

struct NavBuildSettings {
   // Cell size in world units
   float cellSize;
   // Cell height in world units
   float cellHeight;
   // Agent height in world units
   float agentHeight;
   // Agent radius in world units
   float agentRadius;
   // Agent max climb in world units
   float agentMaxClimb;
   // Agent max slope in degrees
   float agentMaxSlope;
   // Region minimum size in voxels.
   // regionMinSize = sqrt(regionMinArea)
   float regionMinSize;
   // Region merge size in voxels.
   // regionMergeSize = sqrt(regionMergeArea)
   float regionMergeSize;
   // Edge max length in world units
   float edgeMaxLen;
   // Edge max error in voxels
   float edgeMaxError;
   float vertsPerPoly;
   // Detail sample distance in voxels
   float detailSampleDist;
   // Detail sample max error in voxel heights.
   float detailSampleMaxError;
   // Partition type, see SamplePartitionType
   int partitionType;
   // Bounds of the area to mesh
   float navMeshBMin[3];
   float navMeshBMax[3];
   // Size of the tiles in voxels
   float tileSize;
};

class NavMeshInputGeometry {
public:
   NavMeshInputGeometry();
   ~NavMeshInputGeometry();

   NavMeshInputGeometry(const NavMeshInputGeometry&) = delete;
   NavMeshInputGeometry& operator=(const NavMeshInputGeometry&) = delete;

   bool SetPreparedMesh(std::shared_ptr<NavInputMeshesManager> mesh);

   /// Method to return static mesh data.
   std::shared_ptr<NavInputMeshesManager> GetMesh() const { return m_pMesh; }

   const float* GetMeshBoundsMin() const { return m_meshBMin; }

   const float* GetMeshBoundsMax() const { return m_meshBMax; }

   const float* GetNavMeshBoundsMin() const { return m_hasBuildSettings ? m_buildSettings.navMeshBMin : m_meshBMin; }

   const float* GetNavMeshBoundsMax() const { return m_hasBuildSettings ? m_buildSettings.navMeshBMax : m_meshBMax; }

   const NavChunkyTriMesh* GetChunkyMesh() const { return m_pChunkyMesh; }

   const NavBuildSettings* GetBuildSettings() const { return m_hasBuildSettings ? &m_buildSettings : 0; }

   bool RaycastMesh(float* src, float* dst, float& tmin);

private:
   NavChunkyTriMesh* m_pChunkyMesh;
   std::shared_ptr<NavInputMeshesManager> m_pMesh;
   float m_meshBMin[3], m_meshBMax[3];
   NavBuildSettings m_buildSettings;
   bool m_hasBuildSettings;
};

} // namespace BIEngine
