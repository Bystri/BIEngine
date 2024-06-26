#pragma once

#include <glm/glm.hpp>

namespace BIEngine {

struct Vertex {
   Vertex()
      : Position(0.0f),
        Normal(0.0f),
        Tangent(0.0f),
        TexCoords(0.0f)
   {
   }

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

} // namespace BIEngine
