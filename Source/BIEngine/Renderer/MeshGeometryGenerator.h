#pragma once

#include <glm/glm.hpp>

#include "../Renderer/Mesh.h"

namespace BIEngine {

class MeshGeometryGenerator {
public:
   static Mesh CreateQuad(float width, float height);
   static Mesh CreateBox(float width, float height, float depth, unsigned int numSubdivisions);
   static Mesh CreateGrid(float width, float depth, unsigned int m, unsigned int n);

private:
   static void Subdivide(Mesh& meshData);
   static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};

} // namespace BIEngine
