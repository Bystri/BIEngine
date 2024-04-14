#pragma once

#include <vector>

#include "Vertex.h"
#include "Texture.h"

namespace BIEngine {

class SkeletalMesh {
public:
   struct VertexBoneData {
      static constexpr int NUM_BONES_PER_VERTEX = 4;

      VertexBoneData()
      {
         for (int i = 0; i < VertexBoneData::NUM_BONES_PER_VERTEX; i++) {
            IDs[i] = -1;
            Weights[i] = 0.0f;
         }
      }

      int IDs[NUM_BONES_PER_VERTEX];
      float Weights[NUM_BONES_PER_VERTEX];
   };

public:
   SkeletalMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<VertexBoneData>& bones);

   unsigned int GetVao() const { return m_VAO; }

   const std::vector<Vertex>& GetVertices() const { return m_vertices; }

   const std::vector<unsigned int>& GetIndices() const { return m_indices; }

private:
   void setupMesh();

private:
   std::vector<Vertex> m_vertices;
   std::vector<unsigned int> m_indices;
   std::vector<VertexBoneData> m_bones;

   unsigned int m_VAO, m_VBO, m_EBO, m_BBO;
};

} // namespace BIEngine
