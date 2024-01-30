#pragma once

#include "../Renderer/Renderer.h"

namespace BIEngine {

class DebugDraw {
public:
   static void Init();

   static void Draw();

   static void Line(const glm::vec3& fromPoint, const glm::vec3& toPoint, const ColorRgba& color);
   static void Poly(const std::vector<glm::vec3>& verts, const ColorRgba& color);
};
} // namespace BIEngine
