#pragma once

#include <array>
#include <vector>

#include "Vector.h"
#include "Matrix.h"

namespace BIEngine {

template <typename T, std::size_t Dim, std::size_t N>
class SplineT {
public:
   SplineT(const std::vector<Vector<T, Dim>>& ctrlPoints)
      : m_ctrlPoints(ctrlPoints)
   {
      if (m_ctrlPoints.size() < N) {
         Assert(false, "Too little points provided for spline; Expected %d", N);
         m_ctrlPoints.clear();
      }
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const = 0;

protected:
   std::vector<Vector<T, Dim>> m_ctrlPoints;
};

// Deg: 1
// Cont: C0
// Tangents: auto
// Interpol: all
template <typename T, std::size_t Dim>
class LinearSplineT : public SplineT<T, Dim, 2> {
public:
   LinearSplineT(const std::vector<Vector<T, Dim>>& ctrlPoints)
      : SplineT(ctrlPoints)
   {
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const override
   {
      Assert(m_ctrlPoints.size() > 0, "No points for interpolation");

      const int idx = static_cast<int>(t);
      Assert(idx < m_ctrlPoints.size() - 1 && idx >= 0, "Provided interpolation param is out of range [0, %d)", m_ctrlPoints.size());

      const T u = t - static_cast<T>(idx);

      return m_ctrlPoints[idx] + u * (m_ctrlPoints[idx + 1] - m_ctrlPoints[idx]);
   }
};

// Deg: 3
// Cont: C0/C1
// Tangents: explicit
// Interpol: all
template <typename T, std::size_t Dim>
class HermiteSplineT : public SplineT<T, Dim, 4> {
public:
   HermiteSplineT(const std::vector<Vector<T, Dim>>& points, const std::vector<Vector<T, Dim>>& vels)
      : SplineT(constructCtrlPoints(points, vels))
   {
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const override
   {
      Assert(m_ctrlPoints.size() > 0, "No points for interpolation");

      int idx = static_cast<int>(t);
      Assert(idx < m_ctrlPoints.size() / 2 - 1 && idx >= 0, "Provided interpolation param is out of range [0, %d)", m_ctrlPoints.size());

      static const Matrix<T, 4, 4> B = {{1.0f, 0.0f, -3.0f, 2.0f}, {0.0f, 1.0f, -2.0f, 1.0f}, {0.0f, 0.0f, 3.0f, -2.0f}, {0.0f, 0.0f, -1.0f, 1.0f}};

      const T u = t - static_cast<T>(idx);
      const Vector<T, 4> tPows = {1, u, u * u, u * u * u};

      idx *= 2;

      Vector<T, Dim> ret;
      for (int i = 0; i < 4; ++i) {
         ret += Dot(B[i], tPows) * m_ctrlPoints[idx + i];
      }

      return ret;
   }

private:
   static std::vector<Vector<T, Dim>> constructCtrlPoints(const std::vector<Vector<T, Dim>>& points, const std::vector<Vector<T, Dim>>& vels)
   {
      std::vector<Vector<T, Dim>> ctrlPoints;

      Assert(points.size() == vels.size(), "The number of points and velocities for Hermite spline must be equal!");

      for (int i = 0; i < points.size(); ++i) {
         ctrlPoints.push_back(points[i]);
         ctrlPoints.push_back(vels[i]);
      }

      return ctrlPoints;
   }
};

// Deg: 3
// Cont: C1
// Tangents: auto
// Interpol: all
template <typename T, std::size_t Dim>
class CatmullRomSplineT : public SplineT<T, Dim, 4> {
public:
   CatmullRomSplineT(const std::vector<Vector<T, Dim>>& points)
      : SplineT(constructCtrlPoints(points))
   {
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const override
   {
      Assert(m_ctrlPoints.size() > 0, "No points for interpolation");

      const int idx = static_cast<int>(t);
      Assert(idx < m_ctrlPoints.size() - 1 && idx >= 0, "Provided interpolation param is out of range [0, %d)", m_ctrlPoints.size());

      static const Matrix<T, 4, 4> B = {{0.0f, -0.5f, 1.0f, -0.5f}, {1.0f, 0.0f, -2.5f, 1.5f}, {0.0f, 0.5f, 2.0f, -1.5f}, {0.0f, 0.0f, -0.5f, 0.5f}};

      const T u = t - static_cast<T>(idx);
      const Vector<T, 4> tPows = {1, u, u * u, u * u * u};

      Vector<T, Dim> ret;
      for (int i = 0; i < 4; ++i) {
         ret += Dot(B[i], tPows) * m_ctrlPoints[idx + i];
      }

      return ret;
   }

private:
   static std::vector<Vector<T, Dim>> constructCtrlPoints(const std::vector<Vector<T, Dim>>& points)
   {
      std::vector<Vector<T, Dim>> ctrlPoints;

      ctrlPoints.push_back(points[0] + (points[0] - points[1]));

      for (int i = 0; i < points.size() - 3; ++i) {
         ctrlPoints.push_back(points[i]);
         ctrlPoints.push_back(points[i + 1]);
         ctrlPoints.push_back(points[i + 2]);
         ctrlPoints.push_back(points[i + 3]);
      }

      ctrlPoints.push_back(points[points.size() - 1] - (points[points.size() - 2] - points[points.size() - 1]));

      return ctrlPoints;
   }
};

// Deg: 3
// Cont: C2
// Tangents: auto
// Interpol: all
template <typename T, std::size_t Dim>
class BSplineT : public SplineT<T, Dim, 4> {
public:
   BSplineT(const std::vector<Vector<T, Dim>>& points)
      : SplineT(points)
   {
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const override
   {
      Assert(m_ctrlPoints.size() > 0, "No points for interpolation");

      const int idx = static_cast<int>(t);
      Assert(idx < m_ctrlPoints.size() - 1 && idx >= 0, "Provided interpolation param is out of range [0, %d)", m_ctrlPoints.size());

      static const Matrix<T, 4, 4> B = {{-1.0f / 6.0f, 0.5f, -0.5f, 1.0f / 6.0f}, {0.5f, -1.0f, 0.0f, 2.0f / 3.0f}, {-0.5f, 0.5f, 0.5f, 1.0f / 6.0f}, {1.0f / 6.0f, 0.0f, 0.0f, 0.0f}};
      const T u = t - static_cast<T>(idx);
      const Vector<T, 4> tPows = {u * u * u, u * u, u, 1.0f};

      Vector<T, Dim> ret;
      for (int i = 0; i < 4; ++i) {
         ret += Dot(B[i], tPows) * m_ctrlPoints[idx + i];
      }

      return ret;
   }
};

// Deg: 3
// Cont: C0/C1
// Tangents: manual
// Interpol: some
template <typename T, std::size_t Dim>
class BezierSplineT : public SplineT<T, Dim, 4> {
public:
   BezierSplineT(const std::vector<Vector<T, Dim>>& points)
      : SplineT(points)
   {
   }

   virtual Vector<T, Dim> GetPoint(const T& t) const override
   {
      Assert(m_ctrlPoints.size() > 0, "No points for interpolation");

      int idx = static_cast<int>(t);
      Assert(idx <= m_ctrlPoints.size() / 4 && idx >= 0, "Provided interpolation param is out of range [0, %d]", m_ctrlPoints.size() / 4);

      static const Matrix<T, 4, 4> B = {{1.0f, -3.0f, 3.0f, -1.0f}, {0.0f, 3.0f, -6.0f, 3.0f}, {0.0f, 0.0f, 3.0f, -3.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
      const T u = t - static_cast<T>(idx);
      const Vector<T, 4> tPows = {1, u, u * u, u * u * u};

      if (idx > 0) {
         idx *= 4;
         idx -= 1;
      }

      Vector<T, Dim> ret;
      for (int i = 0; i < 4; ++i) {
         ret += Dot(B[i], tPows) * m_ctrlPoints[idx + i];
      }

      return ret;
   }
};

using LinearSpline2d = LinearSplineT<float, 2>;
using LinearSpline = LinearSplineT<float, 3>;
using HermiteSpline2d = HermiteSplineT<float, 2>;
using HermiteSpline = HermiteSplineT<float, 3>;
using CatmullRomSpline2d = CatmullRomSplineT<float, 2>;
using CatmullRomSpline = CatmullRomSplineT<float, 3>;
using BSpline2d = BSplineT<float, 2>;
using BSpline = BSplineT<float, 3>;
using BezierSpline2d = BezierSplineT<float, 2>;
using BezierSpline = BezierSplineT<float, 3>;

} // namespace BIEngine
