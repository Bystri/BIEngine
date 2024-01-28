#include "NavInputMeshesManager.h"

#include <cstring>

namespace BIEngine {

NavInputMeshesManager::NavInputMeshesManager()
   : m_pVerts(nullptr),
     m_pTris(nullptr),
     m_pNormals(nullptr),
     m_vertCount(0),
     m_normalCount(0),
     m_triCount(0),
     m_vcap(0),
     m_ncap(0),
     m_icap(0)
{
}

NavInputMeshesManager::~NavInputMeshesManager()
{
   if (m_pVerts) {
      delete[] m_pVerts;
      m_pVerts = nullptr;
   }

   if (m_pNormals) {
      delete[] m_pNormals;
      m_pNormals = nullptr;
   }

   if (m_pTris) {
      delete[] m_pTris;
      m_pTris = nullptr;
   }
}

void NavInputMeshesManager::AddMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh)
{
   const int initialVertsSize = m_vertCount;

   const std::vector<Vertex>& verts = mesh->GetVertices();

   for (int i = 0; i < verts.size(); ++i) {
      const glm::vec3 transformedVert = transform * glm::vec4(verts[i].Position, 1.0f);

      addVertex(transformedVert.x, transformedVert.y, transformedVert.z);
   }

   const std::vector<unsigned int>& inds = mesh->GetIndices();

   for (int i = 0; i < inds.size(); i += 3) {
      addTriangle(initialVertsSize + inds[i] + 1, initialVertsSize + inds[i + 1] + 1, initialVertsSize + inds[i + 2] + 1);
   }
}

void NavInputMeshesManager::addVertex(float x, float y, float z)
{
   if (m_vertCount + 1 > m_vcap) {
      m_vcap = !m_vcap ? 8 : m_vcap * 2;
      float* nv = new float[m_vcap * 3];
      if (m_vertCount) {
         std::memcpy(nv, m_pVerts, m_vertCount * 3 * sizeof(float));
      }
      delete[] m_pVerts;
      m_pVerts = nv;
   }
   float* dst = &m_pVerts[m_vertCount * 3];
   *dst++ = x;
   *dst++ = y;
   *dst++ = z;
   m_vertCount++;
}

void NavInputMeshesManager::addNormal(float x, float y, float z)
{
   if (m_normalCount + 1 > m_ncap) {
      m_ncap = !m_ncap ? 8 : m_ncap * 2;
      float* nv = new float[m_ncap * 3];
      if (m_normalCount) {
         std::memcpy(nv, m_pNormals, m_normalCount * 3 * sizeof(float));
      }
      delete[] m_pNormals;
      m_pNormals = nv;
   }
   float* dst = &m_pNormals[m_normalCount * 3];
   *dst++ = x;
   *dst++ = y;
   *dst++ = z;
   m_normalCount++;
}

void NavInputMeshesManager::addTriangle(int a, int b, int c)
{
   if (m_triCount + 1 > m_icap) {
      m_icap = !m_icap ? 8 : m_icap * 2;
      int* nv = new int[m_icap * 3];
      if (m_triCount) {
         std::memcpy(nv, m_pTris, m_triCount * 3 * sizeof(int));
      }
      delete[] m_pTris;
      m_pTris = nv;
   }
   int* dst = &m_pTris[m_triCount * 3];
   *dst++ = a;
   *dst++ = b;
   *dst++ = c;
   m_triCount++;

   // Add normal

   const float* v0 = &m_pVerts[a];
   const float* v1 = &m_pVerts[b];
   const float* v2 = &m_pVerts[c];
   float e0[3], e1[3];
   for (int j = 0; j < 3; ++j) {
      e0[j] = v1[j] - v0[j];
      e1[j] = v2[j] - v0[j];
   }
   float n[3];
   n[0] = e0[1] * e1[2] - e0[2] * e1[1];
   n[1] = e0[2] * e1[0] - e0[0] * e1[2];
   n[2] = e0[0] * e1[1] - e0[1] * e1[0];
   float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
   if (d > 0) {
      d = 1.0f / d;
      n[0] *= d;
      n[1] *= d;
      n[2] *= d;
   }

   addNormal(n[0], n[1], n[2]);
}


} // namespace BIEngine
