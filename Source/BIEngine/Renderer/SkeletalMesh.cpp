#include "SkeletalMesh.h"

#include <glad/glad.h>

namespace BIEngine {

SkeletalMesh::SkeletalMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<VertexBoneData>& bones)
   : m_vertices(vertices), m_indices(indices), m_bones(bones)
{
   setupMesh();
}

void SkeletalMesh::setupMesh()
{
   glGenVertexArrays(1, &m_VAO);
   glGenBuffers(1, &m_VBO);
   glGenBuffers(1, &m_EBO);
   glGenBuffers(1, &m_BBO);

   glBindVertexArray(m_VAO);

   glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
   glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

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

   glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
   glBufferData(GL_ARRAY_BUFFER, m_bones.size() * sizeof(VertexBoneData), m_bones.data(), GL_STATIC_DRAW);

   // Bone Ids
   glEnableVertexAttribArray(4);
   glVertexAttribIPointer(4, VertexBoneData::NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, IDs));

   // Bone Weights
   glEnableVertexAttribArray(5);
   glVertexAttribPointer(5, VertexBoneData::NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, Weights));

   glBindVertexArray(0);
}
} // namespace BIEngine
