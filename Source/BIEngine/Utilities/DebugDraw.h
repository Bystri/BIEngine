#pragma once

#include "../Renderer/Renderer.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class DebugDraw {
public:
   static void Init();

   static void Draw(const GameTimer& gt);

   static void Sphere(const glm::vec3& center, const float radius, const ColorRgba& color, float time = 0.0f);
   static void Line(const glm::vec3& fromPoint, const glm::vec3& toPoint, const ColorRgba& color, float time = 0.0f);
   static void Poly(const DynamicArray<glm::vec3>& verts, const ColorRgba& color, float time = 0.0f);
};
} // namespace BIEngine
