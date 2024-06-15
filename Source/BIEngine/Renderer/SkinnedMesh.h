#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Vertex.h"
#include "Texture.h"

namespace BIEngine {

class Skeleton;

class SkinnedMesh {
public:
   struct VertexBoneData {
      static constexpr int NUM_BONES_PER_VERTEX = 4;

      VertexBoneData()
      {
         for (int i = 0; i < VertexBoneData::NUM_BONES_PER_VERTEX; i++) {
            boneNames[i] = "";
            Weights[i] = 0.0f;
         }
      }

      std::string boneNames[NUM_BONES_PER_VERTEX];
      float Weights[NUM_BONES_PER_VERTEX];
   };

public:
   SkinnedMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<VertexBoneData>& bones);

   unsigned int GetVao() const { return m_VAO; }

   const std::vector<Vertex>& GetVertices() const { return m_animatedVertices; }

   const std::vector<unsigned int>& GetIndices() const { return m_indices; }

   void OnRender(Skeleton* pSkeleton);

private:
   void setupMesh();

private:
   std::vector<Vertex> m_vertices;
   std::vector<Vertex> m_animatedVertices;
   std::vector<unsigned int> m_indices;
   std::vector<VertexBoneData> m_bones;

   unsigned int m_VAO, m_VBO, m_EBO, m_BBO;
};

} // namespace BIEngine
