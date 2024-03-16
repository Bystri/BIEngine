#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../Renderer/Texture.h"

namespace BIEngine {

struct Vertex {
   Vertex() {}

   Vertex(
      const glm::vec3& p,
      const glm::vec3& n,
      const glm::vec3& t,
      const glm::vec2& uv)
      : Position(p),
        Normal(n),
        Tangent(t),
        TexCoords(uv) {}

   Vertex(
      float px, float py, float pz,
      float nx, float ny, float nz,
      float tx, float ty, float tz,
      float u, float v)
      : Position(px, py, pz),
        Normal(nx, ny, nz),
        Tangent(tx, ty, tz),
        TexCoords(u, v) {}

   glm::vec3 Position;
   glm::vec3 Normal;
   glm::vec3 Tangent;
   glm::vec2 TexCoords;
};

class Mesh {
   friend class MeshGeometryGenerator;

public:
   Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

   unsigned int GetVao() const { return m_VAO; }

   const std::vector<Vertex>& GetVertices() const { return m_vertices; }

   const std::vector<unsigned int>& GetIndices() const { return m_indices; }

private:
   void setupMesh();

private:
   std::vector<Vertex> m_vertices;
   std::vector<unsigned int> m_indices;

   unsigned int m_VAO, m_VBO, m_EBO;
};

} // namespace BIEngine
