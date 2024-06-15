#include "SkinnedMesh.h"

#include <unordered_map>

#include <glad/glad.h>

#include "Skeleton.h"

namespace BIEngine {

class Skeleton;

SkinnedMesh::SkinnedMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<VertexBoneData>& bones)
   : m_vertices(vertices), m_animatedVertices(vertices), m_indices(indices), m_bones(bones)
{
   setupMesh();
}

void SkinnedMesh::setupMesh()
{
   glGenVertexArrays(1, &m_VAO);
   glGenBuffers(1, &m_VBO);
   glGenBuffers(1, &m_EBO);
   glGenBuffers(1, &m_BBO);

   glBindVertexArray(m_VAO);

   glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
   glBufferData(GL_ARRAY_BUFFER, m_animatedVertices.size() * sizeof(Vertex), m_animatedVertices.data(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

   // Позиции вершин
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
   // Нормали вершин
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));

   // Координаты текстур вершин
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Tangent)));

   // Координаты текстур вершин
   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));

   glBindVertexArray(0);
}

void SkinnedMesh::OnRender(Skeleton* pSkeleton)
{
   for (int i = 0; i < m_animatedVertices.size(); ++i) {
      m_animatedVertices[i] = m_vertices[i];

      glm::mat4 finalVertexTransform = glm::mat4(0.0f);
      bool isVertexAnimated = false;

      for (int inflBornId = 0; inflBornId < VertexBoneData::NUM_BONES_PER_VERTEX; ++inflBornId) {
         if (m_bones[i].boneNames[inflBornId] == "") {
            continue;
         }

         finalVertexTransform += pSkeleton->GetBoneMatrix(m_bones[i].boneNames[inflBornId]) * m_bones[i].Weights[inflBornId];
         isVertexAnimated = true;
      }

      if (isVertexAnimated) {
         m_animatedVertices[i].Position = finalVertexTransform * glm::vec4(m_vertices[i].Position, 1.0f);
         m_animatedVertices[i].Tangent = finalVertexTransform * glm::vec4(m_vertices[i].Tangent, 0.0f);
         m_animatedVertices[i].Normal = finalVertexTransform * glm::vec4(m_vertices[i].Normal, 0.0f);
      }
   }

   glBindVertexArray(m_VAO);

   glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
   glBufferData(GL_ARRAY_BUFFER, m_animatedVertices.size() * sizeof(Vertex), m_animatedVertices.data(), GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   glBindVertexArray(0);
}


} // namespace BIEngine
