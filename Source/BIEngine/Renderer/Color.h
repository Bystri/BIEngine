#pragma once

#include <array>

namespace BIEngine {

template <typename T, std::size_t n>
struct Color {
   Color()
      : data()
   {
   }

   std::array<T, n> data;
};

template <typename T>
struct Color<T, 3> {
   Color()
      : r(1.0f), g(1.0f), b(1.0f)
   {
   }

   Color(const float red, const float green, const float blue)
      : r(red), g(green), b(blue)
   {
   }

   union {
      std::array<T, 3> data;

      struct {
         T r;
         T g;
         T b;
      };
   };
};

typedef Color<float, 3> ColorRgb;

template <typename T>
struct Color<T, 4> {
   Color()
      : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
   {
   }

   Color(const float red, const float green, const float blue, const float alpha)
      : r(red), g(green), b(blue), a(alpha)
   {
   }

   Color(const ColorRgb& rgb)
      : r(rgb.r), g(rgb.g), b(rgb.b), a(1.0f)
   {
   }

   union {
      std::array<T, 4> data;

      struct {
         T r;
         T g;
         T b;
         T a;
      };
   };
};

typedef Color<float, 4> ColorRgba;

template <typename T, std::size_t n>
inline const Color<T, n> operator+(const Color<T, n>& lhs, const Color<T, n>& rhs)
{
   Color<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] += rhs.data[i];
   }

   return res;
}

template <typename T, std::size_t n>
inline const Color<T, n> operator-(const Color<T, n>& lhs, const Color<T, n>& rhs)
{
   Color<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] -= rhs.data[i];
   }

   return res;
}

template <typename T, std::size_t n>
inline const Color<T, n> operator*(const Color<T, n>& lhs, const Color<T, n>& rhs)
{
   Color<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] *= rhs.data[i];
   }

   return res;
}

template <typename T, std::size_t n>
inline const Color<T, n> operator*(const Color<T, n>& lhs, const T& scalar)
{
   Color<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] *= scalar;
   }

   return res;
}

template <typename T, std::size_t n>
inline const Color<T, n> operator/(const Color<T, n>& lhs, const T& scalar)
{
   Color<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] /= scalar;
   }

   return res;
}

extern const float ALPHA_OPAQUE;
extern const float ALPHA_TRANSPARENT;

extern const ColorRgba COLOR_WHITE;
extern const ColorRgba COLOR_BLACK;
extern const ColorRgba COLOR_CYAN;
extern const ColorRgba COLOR_RED;
extern const ColorRgba COLOR_GREEN;
extern const ColorRgba COLOR_BLUE;
extern const ColorRgba COLOR_YELLOW;
extern const ColorRgba COLOR_GRAY40;
extern const ColorRgba COLOR_GRAY25;
extern const ColorRgba COLOR_GRAY65;

} // namespace BIEngine
