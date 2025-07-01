#pragma once

#include "Vertex.h"
#include "Texture.h"
#include "../StdLib/DynamicArray.h"

namespace BIEngine {

class Mesh {
   friend class MeshGeometryGenerator;

public:
   Mesh(DynamicArray<Vertex> vertices, DynamicArray<unsigned int> indices);

   unsigned int GetVao() const { return m_VAO; }

   const DynamicArray<Vertex>& GetVertices() const { return m_vertices; }

   const DynamicArray<unsigned int>& GetIndices() const { return m_indices; }

private:
   void setupMesh();

private:
   DynamicArray<Vertex> m_vertices;
   DynamicArray<unsigned int> m_indices;

   unsigned int m_VAO, m_VBO, m_EBO;
};

} // namespace BIEngine
