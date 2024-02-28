#pragma once

#include <array>

#include "Vector.h"

namespace BIEngine {

template <typename T, std::size_t n, std::size_t m>
class Matrix {
public:
   Matrix()
      : columns()
   {
   }

   Matrix(const std::initializer_list<std::initializer_list<T>> cols)
      : columns()
   {
      assert(cols.size() == m, "The number of columns ​​in the initializer_list is not equal to the dimension of the matrix");

      auto iter = cols.begin();
      for (int i = 0; i < m; ++i) {
         assert(iter->size() == n, "The number of elements ​​in the initializer_list's column is not equal to the dimension of the matrix");

         columns[i] = Vector<T, n>(*iter);

         ++iter;
      }
   }

   Vector<T, n> operator[](const std::size_t columIdx) const { return columns[columIdx]; }

   Vector<T, n>& operator[](const std::size_t columIdx) { return columns[columIdx]; }

   inline Matrix<T, n, m>& operator+=(const Matrix<T, n, m>& rhs)
   {
      for (int i = 0; i < m; ++i) {
         for (int j = 0; j < n; ++j) {
            data[i][j] += rhs[i][j];
         }
      }

      return *this;
   }

   inline Matrix<T, n, m>& operator-=(const Matrix<T, n, m>& rhs)
   {
      for (int i = 0; i < m; ++i) {
         for (int j = 0; j < n; ++j) {
            data[i][j] -= rhs[i][j];
         }
      }

      return *this;
   }

   template <typename U>
   inline Matrix<T, n, m>& operator*=(const U& rhs)
   {
      for (int i = 0; i < n; ++i) {
         for (int j = 0; j < m; ++j) {
            data[i][j] *= rhs;
         }
      }

      return *this;
   }

private:
   union {
      T data[m][n];

      struct {
         Vector<T, n> columns[m];
      };
   };
};

typedef Matrix<float, 2, 2> Matrix2;
typedef Matrix<float, 3, 3> Matrix3;
typedef Matrix<float, 4, 4> Matrix4;

template <typename T, std::size_t n, std::size_t m>
inline Matrix<T, n, m> operator+(const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
   Matrix<T, n, m> ret;

   for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
         ret[i][j] = lhs[i][j] + rhs[i][j];
      }
   }

   return ret;
}

template <typename T, std::size_t n, std::size_t m>
inline Matrix<T, n, m> operator-(const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
   Matrix<T, n, m> ret;

   for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
         ret[i][j] = lhs[i][j] - rhs[i][j];
      }
   }

   return ret;
}

template <typename T, typename U, std::size_t n, std::size_t m>
inline Matrix<T, n, m> operator*(const Matrix<T, n, m>& lhs, const U& rhs)
{
   Matrix<T, n, m> ret;

   for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
         ret[i][j] = lhs[i][j] * rhs;
      }
   }

   return ret;
}

template <typename T, std::size_t n, std::size_t m>
inline Vector<T, n> operator*(const Matrix<T, n, m>& lhs, const Vector<T, m>& rhs)
{
   Vector<T, n> ret;

   for (int j = 0; j < m; ++j) {
      for (int i = 0; i < n; ++i) {
         ret[i] += lhs[j][i] * rhs[j];
      }
   }

   return ret;
}

template <typename T, std::size_t n, std::size_t m, std::size_t p>
inline Matrix<T, n, m> operator*(const Matrix<T, n, p>& lhs, const Matrix<T, p, m>& rhs)
{
   Matrix<T, n, m> ret;

   for (int col = 0; col < m; ++col) {
      for (int row = 0; row < n; ++row) {
         for (int k = 0; k < p; ++k) {
            const T mult = lhs[k][row] * rhs[col][k];
            ret[col][row] += lhs[k][row] * rhs[col][k];
         }
      }
   }

   return ret;
}

template <typename T, std::size_t n, std::size_t m>
Matrix<T, m, n> Transposed(const Matrix<T, n, m>& mat)
{
   Matrix<T, m, n> ret;

   for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
         ret[i][j] = mat[j][i];
      }
   }

   return ret;
}

template <typename T>
T Determinant(const Matrix<T, 2, 2>& mat)
{
   return mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
}

template <typename T>
T Determinant(const Matrix<T, 3, 3>& mat)
{
   const T cofactor1 = mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2];
   const T cofactor2 = mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2];
   const T cofactor3 = mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2];
   return mat[0][0] * cofactor1 - mat[1][0] * cofactor2 + mat[2][0] * cofactor3;
}

template <typename T>
T Determinant(const Matrix<T, 4, 4>& mat)
{
   const T cofactor1 = (mat[1][1] * (mat[2][2] * mat[3][3] - mat[3][2] * mat[2][3]) - mat[1][2] * (mat[2][1] * mat[3][3] - mat[2][3] * mat[3][1]) + mat[1][3] * (mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1]));
   const T cofactor2 = (mat[1][0] * (mat[2][2] * mat[3][3] - mat[3][2] * mat[2][3]) - mat[1][2] * (mat[2][0] * mat[3][3] - mat[2][3] * mat[3][0]) + mat[1][3] * (mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0]));
   const T cofactor3 = (mat[1][0] * (mat[2][1] * mat[3][3] - mat[3][1] * mat[2][3]) - mat[1][1] * (mat[2][0] * mat[3][3] - mat[2][3] * mat[3][0]) + mat[1][3] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));
   const T cofactor4 = (mat[1][0] * (mat[2][1] * mat[3][2] - mat[3][1] * mat[2][2]) - mat[1][1] * (mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0]) + mat[1][2] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));
   return mat[0][0] * cofactor1 - mat[0][1] * cofactor2 + mat[0][2] * cofactor3 - mat[0][3] * cofactor4;
}

template <typename T>
Matrix<T, 2, 2> Inverse(const Matrix<T, 2, 2>& mat)
{
   Matrix<T, 2, 2> inverse;

   const float det = Determinant(mat);
   if (det == 0) {
      assert(false, "The matrix is not invertible");
      return inverse;
   }

   inverse[0][0] = mat[1][1] / det;
   inverse[0][1] = -mat[1][0] / det;
   inverse[1][0] = -mat[0][1] / det;
   inverse[1][1] = mat[0][0] / det;

   return inverse;
}

template <typename T>
Matrix<T, 3, 3> Inverse(const Matrix<T, 3, 3>& mat)
{
   Matrix<T, 3, 3> inverse;

   const float det = Determinant(mat);
   if (det == 0) {
      assert(false, "The matrix is not invertible");
      return inverse;
   }

   inverse[0][0] = (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) / det;
   inverse[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) / det;
   inverse[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) / det;
   inverse[1][0] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) / det;
   inverse[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) / det;
   inverse[1][2] = (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]) / det;
   inverse[2][0] = (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) / det;
   inverse[2][1] = (mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1]) / det;
   inverse[2][2] = (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) / det;

   return inverse;
}

template <typename T>
Matrix<T, 4, 4> Inverse(const Matrix<T, 4, 4>& mat)
{
   Matrix<T, 4, 4> inverse;

   const float det = Determinant(mat);
   if (det == 0) {
      assert(false, "The matrix is not invertible");
      return inverse;
   }

   T A2323 = mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2];
   T A1323 = mat[2][1] * mat[3][3] - mat[2][3] * mat[3][1];
   T A1223 = mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1];
   T A0323 = mat[2][0] * mat[3][3] - mat[2][3] * mat[3][0];
   T A0223 = mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0];
   T A0123 = mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0];
   T A2313 = mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2];
   T A1313 = mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1];
   T A1213 = mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1];
   T A2312 = mat[1][2] * mat[2][3] - mat[1][3] * mat[2][2];
   T A1312 = mat[1][1] * mat[2][3] - mat[1][3] * mat[2][1];
   T A1212 = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
   T A0313 = mat[1][0] * mat[3][3] - mat[1][3] * mat[3][0];
   T A0213 = mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0];
   T A0312 = mat[1][0] * mat[2][3] - mat[1][3] * mat[2][0];
   T A0212 = mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0];
   T A0113 = mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0];
   T A0112 = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

   inverse[0][0] = (mat[1][1] * A2323 - mat[1][2] * A1323 + mat[1][3] * A1223) / det;
   inverse[0][1] = -(mat[0][1] * A2323 - mat[0][2] * A1323 + mat[0][3] * A1223) / det;
   inverse[0][2] = (mat[0][1] * A2313 - mat[0][2] * A1313 + mat[0][3] * A1213) / det;
   inverse[0][3] = -(mat[0][1] * A2312 - mat[0][2] * A1312 + mat[0][3] * A1212) / det;
   inverse[1][0] = -(mat[1][0] * A2323 - mat[1][2] * A0323 + mat[1][3] * A0223) / det;
   inverse[1][1] = (mat[0][0] * A2323 - mat[0][2] * A0323 + mat[0][3] * A0223) / det;
   inverse[1][2] = -(mat[0][0] * A2313 - mat[0][2] * A0313 + mat[0][3] * A0213) / det;
   inverse[1][3] = (mat[0][0] * A2312 - mat[0][2] * A0312 + mat[0][3] * A0212) / det;
   inverse[2][0] = (mat[1][0] * A1323 - mat[1][1] * A0323 + mat[1][3] * A0123) / det;
   inverse[2][1] = -(mat[0][0] * A1323 - mat[0][1] * A0323 + mat[0][3] * A0123) / det;
   inverse[2][2] = (mat[0][0] * A1313 - mat[0][1] * A0313 + mat[0][3] * A0113) / det;
   inverse[2][3] = -(mat[0][0] * A1312 - mat[0][1] * A0312 + mat[0][3] * A0112) / det;
   inverse[3][0] = -(mat[1][0] * A1223 - mat[1][1] * A0223 + mat[1][2] * A0123) / det;
   inverse[3][1] = (mat[0][0] * A1223 - mat[0][1] * A0223 + mat[0][2] * A0123) / det;
   inverse[3][2] = -(mat[0][0] * A1213 - mat[0][1] * A0213 + mat[0][2] * A0113) / det;
   inverse[3][3] = (mat[0][0] * A1212 - mat[0][1] * A0212 + mat[0][2] * A0112) / det;

   return inverse;
}

} // namespace BIEngine
