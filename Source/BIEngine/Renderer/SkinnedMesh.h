#pragma once

#include "Vertex.h"
#include "Texture.h"
#include "../StdLib/DynamicArray.h"
#include "../StdLib/String.h"

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

      String boneNames[NUM_BONES_PER_VERTEX];
      float Weights[NUM_BONES_PER_VERTEX];
   };

public:
   SkinnedMesh(const DynamicArray<Vertex>& vertices, DynamicArray<unsigned int> indices, DynamicArray<VertexBoneData> bones);
   SkinnedMesh(const SkinnedMesh& other);

   ~SkinnedMesh();

   unsigned int GetVao() const { return m_VAO; }

   const DynamicArray<Vertex>& GetVertices() const { return m_animatedVertices; }

   const DynamicArray<unsigned int>& GetIndices() const { return m_indices; }

   void OnRender(Skeleton* pSkeleton);

private:
   void setupMesh();

private:
   DynamicArray<Vertex> m_vertices;
   DynamicArray<Vertex> m_animatedVertices;
   DynamicArray<unsigned int> m_indices;
   DynamicArray<VertexBoneData> m_bones;

   unsigned int m_VAO, m_VBO, m_EBO, m_BBO;
};

} // namespace BIEngine
