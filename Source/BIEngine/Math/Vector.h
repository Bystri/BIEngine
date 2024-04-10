#pragma once

#include <array>
#include <initializer_list>

#include "Math.h"
#include "../EngineCore/Assert.h"

namespace BIEngine {

template <typename T, std::size_t n>
struct Vector {
   Vector()
      : data()
   {
   }

   explicit Vector(const T& value)
      : data(value)
   {
   }

   Vector(const std::initializer_list<T> args)
      : data()
   {
      Assert(args.size() == n, "The number of values ​​in the initializer_list is not equal to the dimension of the vector");

      auto iter = args.begin();

      for (int i = 0; i < n; ++i) {
         data[i] = *iter;
         ++iter;
      }
   }

   T& operator[](const std::size_t idx) { return data[idx]; }

   T operator[](const std::size_t idx) const { return data[idx]; }

   inline Vector<T, n>& operator+=(const Vector<T, n>& rhs)
   {
      for (int i = 0; i < n; ++i) {
         data[i] += rhs.data[i];
      }

      return *this;
   }

   inline Vector<T, n>& operator-=(const Vector<T, n>& rhs)
   {
      for (int i = 0; i < n; ++i) {
         data[i] -= -rhs.data[i];
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, n>& operator*=(const U& scalar)
   {
      for (int i = 0; i < n; ++i) {
         data[i] *= scalar;
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, n>& operator/=(const U& scalar)
   {
      for (int i = 0; i < n; ++i) {
         data[i] /= scalar;
      }

      return *this;
   }

   std::array<T, n> data;
};

template <typename T>
struct Vector<T, 2> {

   Vector()
      : data()
   {
   }

   explicit Vector(const T& value)
      : data(value)
   {
   }

   Vector(const std::initializer_list<T> args)
      : data()
   {
      Assert(args.size() == 2, "The number of values ​​in the initializer_list is not equal to the dimension of the vector");

      auto iter = args.begin();

      for (int i = 0; i < 2; ++i) {
         data[i] = *iter;
         ++iter;
      }
   }

   Vector(const T& x, const T& y)
   {
      data[0] = x;
      data[1] = y;
   }

   T& operator[](const std::size_t idx) { return data[idx]; }

   T operator[](const std::size_t idx) const { return data[idx]; }

   inline Vector<T, 2>& operator+=(const Vector<T, 2>& rhs)
   {
      for (int i = 0; i < 2; ++i) {
         data[i] += rhs.data[i];
      }

      return *this;
   }

   inline Vector<T, 2>& operator-=(const Vector<T, 2>& rhs)
   {
      for (int i = 0; i < 2; ++i) {
         data[i] -= -rhs.data[i];
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 2>& operator*=(const U& scalar)
   {
      for (int i = 0; i < 2; ++i) {
         data[i] *= scalar;
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 2>& operator/=(const U& scalar)
   {
      for (int i = 0; i < 2; ++i) {
         data[i] /= scalar;
      }

      return *this;
   }

   union {
      std::array<T, 2> data;

      struct
      {
         T x;
         T y;
      };
   };
};

template <typename T>
struct Vector<T, 3> {

   Vector()
      : data()
   {
   }

   explicit Vector(const T& value)
      : data(value)
   {
   }

   Vector(const Vector<T, 2>& vec, const T& z)
      : data()
   {
      data[0] = vec[0];
      data[1] = vec[1];
      data[2] = z;
   }

   Vector(const std::initializer_list<T> args)
      : data()
   {
      Assert(args.size() == 3, "The number of values ​​in the initializer_list is not equal to the dimension of the vector");

      auto iter = args.begin();

      for (int i = 0; i < 3; ++i) {
         data[i] = *iter;
         ++iter;
      }
   }

   Vector(const T& x, const T& y, const T& z)
   {
      data[0] = x;
      data[1] = y;
      data[2] = z;
   }

   T& operator[](const std::size_t idx) { return data[idx]; }

   T operator[](const std::size_t idx) const { return data[idx]; }

   inline Vector<T, 3>& operator+=(const Vector<T, 3>& rhs)
   {
      for (int i = 0; i < 3; ++i) {
         data[i] += rhs.data[i];
      }

      return *this;
   }

   inline Vector<T, 3>& operator-=(const Vector<T, 3>& rhs)
   {
      for (int i = 0; i < 3; ++i) {
         data[i] -= -rhs.data[i];
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 3>& operator*=(const U& scalar)
   {
      for (int i = 0; i < 3; ++i) {
         data[i] *= scalar;
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 3>& operator/=(const U& scalar)
   {
      for (int i = 0; i < 3; ++i) {
         data[i] /= scalar;
      }

      return *this;
   }

   union {
      std::array<T, 3> data;

      struct
      {
         T x;
         T y;
         T z;
      };
   };
};

template <typename T>
struct Vector<T, 4> {

   Vector()
      : data()
   {
   }

   explicit Vector(const T& value)
      : data(value)
   {
   }

   Vector(const Vector<T, 3>& vec, const T& w)
      : data()
   {
      data[0] = vec[0];
      data[1] = vec[1];
      data[2] = vec[2];
      data[3] = w;
   }

   Vector(const std::initializer_list<T> args)
      : data()
   {
      Assert(args.size() == 4, "The number of values ​​in the initializer_list is not equal to the dimension of the vector");

      auto iter = args.begin();

      for (int i = 0; i < 4; ++i) {
         data[i] = *iter;
         ++iter;
      }
   }

   Vector(const T& x, const T& y, const T& z, const T& w)
   {
      data[0] = x;
      data[1] = y;
      data[2] = y;
      data[3] = w;
   }

   T& operator[](const std::size_t idx) { return data[idx]; }

   T operator[](const std::size_t idx) const { return data[idx]; }

   inline Vector<T, 4>& operator+=(const Vector<T, 4>& rhs)
   {
      for (int i = 0; i < 4; ++i) {
         data[i] += rhs.data[i];
      }

      return *this;
   }

   inline Vector<T, 4>& operator-=(const Vector<T, 4>& rhs)
   {
      for (int i = 0; i < 4; ++i) {
         data[i] -= -rhs.data[i];
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 4>& operator*=(const U& scalar)
   {
      for (int i = 0; i < 4; ++i) {
         data[i] *= scalar;
      }

      return *this;
   }

   template <typename U>
   inline Vector<T, 4>& operator/=(const U& scalar)
   {
      for (int i = 0; i < 4; ++i) {
         data[i] /= scalar;
      }

      return *this;
   }

   union {
      std::array<T, 4> data;

      struct
      {
         T x;
         T y;
         T z;
         T w;
      };
   };
};

typedef Vector<float, 2> Vector2;
typedef Vector<float, 3> Vector3;
typedef Vector<float, 4> Vector4;

template <typename T, std::size_t n>
inline const Vector<T, n> operator-(const Vector<T, n>& rhs)
{
   Vector<T, n> res = rhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] = -res.data[i];
   }

   return res;
}

template <typename T, std::size_t n>
inline const Vector<T, n> operator+(const Vector<T, n>& lhs, const Vector<T, n>& rhs)
{
   Vector<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] += rhs.data[i];
   }

   return res;
}

template <typename T, std::size_t n>
inline const Vector<T, n> operator-(const Vector<T, n>& lhs, const Vector<T, n>& rhs)
{
   Vector<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] -= rhs.data[i];
   }

   return res;
}

template <typename T, typename U, std::size_t n>
inline const Vector<T, n> operator*(const Vector<T, n>& lhs, const U& scalar)
{
   Vector<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] *= scalar;
   }

   return res;
}

template <typename T, typename U, std::size_t n>
inline const Vector<T, n> operator*(const U& scalar, const Vector<T, n>& rhs)
{
   return rhs * scalar;
}

template <typename T, typename U, std::size_t n>
inline const Vector<T, n> operator/(const Vector<T, n>& lhs, const U& scalar)
{
   Vector<T, n> res = lhs;

   for (int i = 0; i < n; ++i) {
      res.data[i] /= scalar;
   }

   return res;
}

template <typename T, std::size_t n>
inline T Length2(const Vector<T, n>& vector)
{
   T length2 = {};

   for (int i = 0; i < n; ++i) {
      length2 += vector.data[i] * vector.data[i];
   }

   return length2;
}

template <typename T, std::size_t n>
inline T Length(const Vector<T, n>& vector)
{
   return Sqrt(Length2(vector));
}

template <typename T, std::size_t n>
inline void Normalize(Vector<T, n>& vector)
{
   const T l = Length(vector);

   for (int i = 0; i < n; ++i) {
      vector[i] /= l;
   }
}

template <typename T, std::size_t n>
inline Vector<T, n> Normalized(const Vector<T, n>& vector)
{
   Vector<T, n> norm = vector;
   Normalize(norm);

   return norm;
}

template <typename T, std::size_t n>
inline T Dot(const Vector<T, n>& lhs, const Vector<T, n>& rhs)
{
   T scalar = {};

   for (int i = 0; i < n; ++i) {
      scalar += lhs.data[i] * rhs.data[i];
   }

   return scalar;
}

template <typename T>
inline Vector<T, 3> Cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
{
   Vector<T, 3> retVec = {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x};

   return retVec;
}

} // namespace BIEngine
