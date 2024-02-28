#include "Trigonometry.h"

#include <cmath>

namespace BIEngine {

float RadToDeg(const float rad)
{
   static constexpr float factor = 180.0f / PI;
   return factor * rad;
}

float DegToRad(const float deg)
{
   static constexpr float factor = PI / 180.0f;
   return factor * deg;
}

float Cos(const float rad)
{
   return std::cos(rad);
}

float Sin(const float rad)
{
   return std::sin(rad);
}

float Tan(const float rad)
{
   return std::tan(rad);
}

double RadToDeg(const double rad)
{
   static constexpr double factor = 180.0f / PI_DOUBLE;
   return factor * rad;
}

double DegToRad(const double deg)
{
   static constexpr float factor = PI_DOUBLE / 180.0f;
   return factor * deg;
}

double Cos(const double rad)
{
   return std::cos(rad);
}

double Sin(const double rad)
{
   return std::sin(rad);
}

double Tan(const double rad)
{
   return std::tan(rad);
}

} // namespace BIEngine
