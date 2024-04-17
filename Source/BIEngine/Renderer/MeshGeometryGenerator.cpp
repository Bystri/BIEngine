#include "MeshGeometryGenerator.h"

#include <algorithm>

namespace BIEngine {

Mesh MeshGeometryGenerator::CreateQuad(float width, float height)
{
   std::vector<Vertex> v;
   v.resize(4);

   // Передняя сторона
   v[0] = Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   v[1] = Vertex(0.0f, height, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   v[2] = Vertex(width, height, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
   v[3] = Vertex(width, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);


   // Индексы
   std::vector<unsigned int> i;
   i.resize(6);

   // Передняя сторона
   i[0] = 0;
   i[1] = 1;
   i[2] = 2;
   i[3] = 0;
   i[4] = 2;
   i[5] = 3;

   Mesh meshData(v, i);

   return meshData;
}

Mesh MeshGeometryGenerator::CreateBox(float width, float height, float depth, unsigned int numSubdivisions)
{
   // Вершины
   std::vector<Vertex> v;
   v.resize(24);

   float w2 = 0.5f * width;
   float h2 = 0.5f * height;
   float d2 = 0.5f * depth;

   // Передняя сторона
   v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
   v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

   // Задняя сторона
   v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
   v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

   // Верхняя сторона
   v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
   v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

   // Нижняя сторона
   v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
   v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

   // Левая сторона
   v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
   v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
   v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
   v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

   // Правая сторона
   v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
   v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
   v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
   v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

   // Индексы
   std::vector<unsigned int> i;
   i.resize(36);

   // Передняя сторона
   i[0] = 0;
   i[1] = 1;
   i[2] = 2;
   i[3] = 0;
   i[4] = 2;
   i[5] = 3;

   // Задняя сторона
   i[6] = 4;
   i[7] = 5;
   i[8] = 6;
   i[9] = 4;
   i[10] = 6;
   i[11] = 7;

   // Верхняя сторона
   i[12] = 8;
   i[13] = 9;
   i[14] = 10;
   i[15] = 8;
   i[16] = 10;
   i[17] = 11;

   // Нижняя сторона
   i[18] = 12;
   i[19] = 13;
   i[20] = 14;
   i[21] = 12;
   i[22] = 14;
   i[23] = 15;

   // Левая сторона
   i[24] = 16;
   i[25] = 17;
   i[26] = 18;
   i[27] = 16;
   i[28] = 18;
   i[29] = 19;

   // Правая сторона
   i[30] = 20;
   i[31] = 21;
   i[32] = 22;
   i[33] = 20;
   i[34] = 22;
   i[35] = 23;

   Mesh meshData(v, i);

   // Берем минимально возможное значени разделений
   numSubdivisions = std::min<unsigned int>(numSubdivisions, 6u);

   for (unsigned int i = 0; i < numSubdivisions; ++i) {
      Subdivide(meshData);
   }

   return meshData;
}

Mesh MeshGeometryGenerator::CreateGrid(float width, float depth, unsigned int m, unsigned int n)
{
   const unsigned int vertexCount = m * n;
   const unsigned int faceCount = (m - 1) * (n - 1) * 2;


   const float halfWidth = 0.5f * width;
   const float halfDepth = 0.5f * depth;

   const float dx = width / (n - 1);
   const float dz = depth / (m - 1);

   const float du = 1.0f / (n - 1);
   const float dv = 1.0f / (m - 1);

   std::vector<Vertex> v(vertexCount);
   for (int i = 0; i < m; ++i) {
      const float z = halfDepth - i * dz;
      for (int j = 0; j < n; ++j) {
         const float x = -halfWidth + j * dx;

         //						Position		Normal			Tangetnt	    TexCoords
         v[i * n + j] = Vertex{x, 0.0f, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, j * du, i * dv};
      }
   }


   std::vector<unsigned int> ind(faceCount * 3);

   int k = 0;
   for (int i = 0; i < m - 1; ++i) {
      for (int j = 0; j < n - 1; ++j) {
         ind[k] = i * n + j;
         ind[k + 1] = i * n + j + 1;
         ind[k + 2] = (i + 1) * n + j;

         ind[k + 3] = (i + 1) * n + j;
         ind[k + 4] = i * n + j + 1;
         ind[k + 5] = (i + 1) * n + j + 1;

         k += 6;
      }
   }

   return Mesh(v, ind);
}

Mesh MeshGeometryGenerator::CreateSphere(float radius, unsigned int sliceCount, unsigned int stackCount)
{
   //
   // Compute the vertices stating at the top pole and moving down the stacks.
   //

   // Poles: note that there will be texture coordinate distortion as there is
   // not a unique point on the texture map to assign to the pole when mapping
   // a rectangular texture onto a sphere.
   const Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   const Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

   std::vector<Vertex> v;

   v.push_back(topVertex);

   constexpr float PI = 3.1415927;

   const float phiStep = PI / stackCount;
   const float thetaStep = 2.0f * PI / sliceCount;

   // Compute vertices for each stack ring (do not count the poles as rings).
   for (unsigned int i = 1; i <= stackCount - 1; ++i) {
      const float phi = i * phiStep;

      // Vertices of ring.
      for (unsigned int j = 0; j <= sliceCount; ++j) {
         const float theta = j * thetaStep;

         Vertex vert;

         // spherical to cartesian
         vert.Position.x = radius * sinf(phi) * cosf(theta);
         vert.Position.y = radius * cosf(phi);
         vert.Position.z = radius * sinf(phi) * sinf(theta);

         // Partial derivative of P with respect to theta
         vert.Tangent.x = -radius * sinf(phi) * sinf(theta);
         vert.Tangent.y = 0.0f;
         vert.Tangent.z = +radius * sinf(phi) * cosf(theta);

         glm::normalize(vert.Tangent);

         vert.Normal = vert.Position;
         glm::normalize(vert.Normal);

         vert.TexCoords.x = theta / PI;
         vert.TexCoords.y = phi / PI;

         v.push_back(vert);
      }
   }

   v.push_back(bottomVertex);

   //
   // Compute indices for top stack.  The top stack was written first to the vertex buffer
   // and connects the top pole to the first ring.
   //

   std::vector<unsigned int> ind;

   for (unsigned i = 1; i <= sliceCount; ++i) {
      ind.push_back(0);
      ind.push_back(i + 1);
      ind.push_back(i);
   }

   //
   // Compute indices for inner stacks (not connected to poles).
   //

   // Offset the indices to the index of the first vertex in the first ring.
   // This is just skipping the top pole vertex.
   unsigned int baseIndex = 1;
   const unsigned int ringVertexCount = sliceCount + 1;
   for (unsigned int i = 0; i < stackCount - 2; ++i) {
      for (unsigned int j = 0; j < sliceCount; ++j) {
         ind.push_back(baseIndex + i * ringVertexCount + j);
         ind.push_back(baseIndex + i * ringVertexCount + j + 1);
         ind.push_back(baseIndex + (i + 1) * ringVertexCount + j);

         ind.push_back(baseIndex + (i + 1) * ringVertexCount + j);
         ind.push_back(baseIndex + i * ringVertexCount + j + 1);
         ind.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
      }
   }

   //
   // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
   // and connects the bottom pole to the bottom ring.
   //

   // South pole vertex was added last.
   const unsigned int southPoleIndex = static_cast<unsigned int>(ind.size()) - 1;

   // Offset the indices to the index of the first vertex in the last ring.
   baseIndex = southPoleIndex - ringVertexCount;

   for (unsigned int i = 0; i < sliceCount; ++i) {
      ind.push_back(southPoleIndex);
      ind.push_back(baseIndex + i);
      ind.push_back(baseIndex + i + 1);
   }

   return Mesh(v, ind);
}

void MeshGeometryGenerator::Subdivide(Mesh& meshData)
{
   Mesh inputCopy = meshData;


   meshData.m_vertices.resize(0);
   meshData.m_indices.resize(0);

   unsigned int numTris = (unsigned int)inputCopy.m_indices.size() / 3;
   for (unsigned int i = 0; i < numTris; ++i) {
      Vertex v0 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 0]];
      Vertex v1 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 1]];
      Vertex v2 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 2]];


      // Генерируем среднии точки
      Vertex m0 = MidPoint(v0, v1);
      Vertex m1 = MidPoint(v1, v2);
      Vertex m2 = MidPoint(v0, v2);


      // Добавляем новую геометрию
      meshData.m_vertices.push_back(v0); // 0
      meshData.m_vertices.push_back(v1); // 1
      meshData.m_vertices.push_back(v2); // 2
      meshData.m_vertices.push_back(m0); // 3
      meshData.m_vertices.push_back(m1); // 4
      meshData.m_vertices.push_back(m2); // 5

      meshData.m_indices.push_back(i * 6 + 0);
      meshData.m_indices.push_back(i * 6 + 3);
      meshData.m_indices.push_back(i * 6 + 5);

      meshData.m_indices.push_back(i * 6 + 3);
      meshData.m_indices.push_back(i * 6 + 4);
      meshData.m_indices.push_back(i * 6 + 5);

      meshData.m_indices.push_back(i * 6 + 5);
      meshData.m_indices.push_back(i * 6 + 4);
      meshData.m_indices.push_back(i * 6 + 2);

      meshData.m_indices.push_back(i * 6 + 3);
      meshData.m_indices.push_back(i * 6 + 1);
      meshData.m_indices.push_back(i * 6 + 4);
   }
}

Vertex MeshGeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
   Vertex v;
   v.Position = 0.5f * (v0.Position + v1.Position);
   v.Normal = glm::normalize(0.5f * (v0.Normal + v1.Normal));
   v.TexCoords = 0.5f * (v0.TexCoords + v1.TexCoords);

   return v;
}
} // namespace BIEngine
