#pragma once

#include <glm/glm.hpp>

#include "NavChunkyTriMesh.h"
#include "NavInputMeshesManager.h"
#include "../Renderer/Mesh.h"

namespace BIEngine {

class NavMeshInputGeometry {
public:
   NavMeshInputGeometry();
   ~NavMeshInputGeometry();

   NavMeshInputGeometry(const NavMeshInputGeometry&) = delete;
   NavMeshInputGeometry& operator=(const NavMeshInputGeometry&) = delete;

   bool SetPreparedMesh(SharedPtr<NavInputMeshesManager> mesh);

   /// Method to return static mesh data.
   SharedPtr<NavInputMeshesManager> GetMesh() const { return m_pMesh; }

   const float* GetMeshBoundsMin() const { return m_meshBMin; }

   const float* GetMeshBoundsMax() const { return m_meshBMax; }

   const NavChunkyTriMesh* GetChunkyMesh() const { return m_pChunkyMesh; }

   bool RaycastMesh(float* src, float* dst, float& tmin);

private:
   NavChunkyTriMesh* m_pChunkyMesh;
   SharedPtr<NavInputMeshesManager> m_pMesh;
   float m_meshBMin[3], m_meshBMax[3];
   bool m_hasBuildSettings;
};

} // namespace BIEngine
