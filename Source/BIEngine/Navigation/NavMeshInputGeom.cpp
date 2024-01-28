#include "NavMeshInputGeom.h"

#include <Recast.h>

#include "../Utilities/Logger.h"
#include "NavInputMeshesManager.h"

namespace BIEngine {

NavMeshInputGeometry::NavMeshInputGeometry()
   : m_pChunkyMesh(nullptr),
     m_pMesh(nullptr),
     m_hasBuildSettings(false)
{
}

NavMeshInputGeometry::~NavMeshInputGeometry()
{
   if (m_pChunkyMesh) {
      delete m_pChunkyMesh;
      m_pChunkyMesh = nullptr;
   }
}

bool NavMeshInputGeometry::SetPreparedMesh(std::shared_ptr<NavInputMeshesManager> mesh)
{
   if (m_pMesh) {
      delete m_pChunkyMesh;
      m_pChunkyMesh = nullptr;
   }

   m_pMesh = mesh;

   rcCalcBounds(m_pMesh->GetVerts(), m_pMesh->GetVertCount(), m_meshBMin, m_meshBMax);

   m_pChunkyMesh = new NavChunkyTriMesh();
   if (!m_pChunkyMesh) {
      Logger::WriteErrorLog("buildTiledNavigation: Out of memory 'm_pChunkyMesh'");
      return false;
   }
   if (!NavCreateChunkyTriMesh(m_pMesh->GetVerts(), m_pMesh->GetTris(), m_pMesh->GetTriCount(), 256, m_pChunkyMesh)) {
      Logger::WriteErrorLog("buildTiledNavigation: Failed to build chunky mesh.");
      return false;
   }

   return true;
}

static bool isectSegAABB(const float* sp, const float* sq, const float* amin, const float* amax, float& tmin, float& tmax)
{
   static const float EPS = 1e-6f;

   float d[3];
   d[0] = sq[0] - sp[0];
   d[1] = sq[1] - sp[1];
   d[2] = sq[2] - sp[2];
   tmin = 0.0;
   tmax = 1.0f;

   for (int i = 0; i < 3; i++) {
      if (fabsf(d[i]) < EPS) {
         if (sp[i] < amin[i] || sp[i] > amax[i])
            return false;
      } else {
         const float ood = 1.0f / d[i];
         float t1 = (amin[i] - sp[i]) * ood;
         float t2 = (amax[i] - sp[i]) * ood;
         if (t1 > t2) {
            float tmp = t1;
            t1 = t2;
            t2 = tmp;
         }
         if (t1 > tmin)
            tmin = t1;
         if (t2 < tmax)
            tmax = t2;
         if (tmin > tmax)
            return false;
      }
   }

   return true;
}

static bool intersectSegmentTriangle(const float* sp, const float* sq, const float* a, const float* b, const float* c, float& t)
{
   float v, w;
   float ab[3], ac[3], qp[3], ap[3], norm[3], e[3];
   rcVsub(ab, b, a);
   rcVsub(ac, c, a);
   rcVsub(qp, sp, sq);

   // Compute triangle normal. Can be precalculated or cached if
   // intersecting multiple segments against the same triangle
   rcVcross(norm, ab, ac);

   // Compute denominator d. If d <= 0, segment is parallel to or points
   // away from triangle, so exit early
   float d = rcVdot(qp, norm);
   if (d <= 0.0f)
      return false;

   // Compute intersection t value of pq with plane of triangle. A ray
   // intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
   // dividing by d until intersection has been found to pierce triangle
   rcVsub(ap, sp, a);
   t = rcVdot(ap, norm);
   if (t < 0.0f)
      return false;
   if (t > d)
      return false; // For segment; exclude this code line for a ray test

   // Compute barycentric coordinate components and test if within bounds
   rcVcross(e, qp, ap);
   v = rcVdot(ac, e);
   if (v < 0.0f || v > d)
      return false;
   w = -rcVdot(ab, e);
   if (w < 0.0f || v + w > d)
      return false;

   // Segment/ray intersects triangle. Perform delayed division
   t /= d;

   return true;
}

bool NavMeshInputGeometry::RaycastMesh(float* src, float* dst, float& tmin)
{
   // Prune hit ray.
   float btmin, btmax;
   if (!isectSegAABB(src, dst, m_meshBMin, m_meshBMax, btmin, btmax))
      return false;
   float p[2], q[2];
   p[0] = src[0] + (dst[0] - src[0]) * btmin;
   p[1] = src[2] + (dst[2] - src[2]) * btmin;
   q[0] = src[0] + (dst[0] - src[0]) * btmax;
   q[1] = src[2] + (dst[2] - src[2]) * btmax;

   int cid[512];
   const int ncid = NavGetChunksOverlappingSegment(m_pChunkyMesh, p, q, cid, 512);
   if (!ncid)
      return false;

   tmin = 1.0f;
   bool hit = false;
   const float* verts = m_pMesh->GetVerts();

   for (int i = 0; i < ncid; ++i) {
      const NavChunkyTriMeshNode& node = m_pChunkyMesh->nodes[cid[i]];
      const int* tris = &m_pChunkyMesh->tris[node.i * 3];
      const int ntris = node.n;

      for (int j = 0; j < ntris * 3; j += 3) {
         float t = 1;
         if (intersectSegmentTriangle(src, dst, &verts[tris[j] * 3], &verts[tris[j + 1] * 3], &verts[tris[j + 2] * 3], t)) {
            if (t < tmin)
               tmin = t;
            hit = true;
         }
      }
   }

   return hit;
}

} // namespace BIEngine
