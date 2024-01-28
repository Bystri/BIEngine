#pragma once

namespace BIEngine {

struct NavChunkyTriMeshNode {
   float bmin[2];
   float bmax[2];
   int i;
   int n;
};

struct NavChunkyTriMesh {
   NavChunkyTriMesh()
      : nodes(0), nnodes(0), tris(0), ntris(0), maxTrisPerChunk(0) {}

   ~NavChunkyTriMesh()
   {
      delete[] nodes;
      delete[] tris;
   }

   NavChunkyTriMesh(const NavChunkyTriMesh&) = delete;
   NavChunkyTriMesh& operator=(const NavChunkyTriMesh&) = delete;

   NavChunkyTriMeshNode* nodes;
   int nnodes;
   int* tris;
   int ntris;
   int maxTrisPerChunk;
};

/// Creates partitioned triangle mesh (AABB tree),
/// where each node contains at max trisPerChunk triangles.
bool NavCreateChunkyTriMesh(const float* verts, const int* tris, int ntris, int trisPerChunk, NavChunkyTriMesh* cm);

/// Returns the chunk indices which overlap the input rectable.
int NavGetChunksOverlappingRect(const NavChunkyTriMesh* cm, float bmin[2], float bmax[2], int* ids, const int maxIds);

/// Returns the chunk indices which overlap the input segment.
int NavGetChunksOverlappingSegment(const NavChunkyTriMesh* cm, float p[2], float q[2], int* ids, const int maxIds);

} // namespace BIEngine
