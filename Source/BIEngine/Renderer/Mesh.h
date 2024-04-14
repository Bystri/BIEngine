#pragma once

#include <vector>

#include "Vertex.h"
#include "Texture.h"

namespace BIEngine {

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
