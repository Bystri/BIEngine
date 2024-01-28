#pragma once

#include "../Renderer/Mesh.h"

#include <glm/glm.hpp>

namespace BIEngine {

class Mesh;

class NavInputMeshesManager {
public:
   NavInputMeshesManager();
   ~NavInputMeshesManager();

   NavInputMeshesManager(const NavInputMeshesManager&) = delete;
   NavInputMeshesManager& operator=(const NavInputMeshesManager&) = delete;

   void AddMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh);

   const float* GetVerts() const { return m_pVerts; }

   const float* GetNormals() const { return m_pNormals; }

   const int* GetTris() const { return m_pTris; }

   int GetVertCount() const { return m_vertCount; }

   int GetTriCount() const { return m_triCount; }

private:
   void addVertex(float x, float y, float z);
   void addNormal(float x, float y, float z);
   void addTriangle(int a, int b, int c);

   float* m_pVerts;
   int* m_pTris;
   float* m_pNormals;
   int m_vertCount;
   int m_normalCount;
   int m_triCount;

   int m_vcap;
   int m_ncap;
   int m_icap;
};

} // namespace BIEngine
