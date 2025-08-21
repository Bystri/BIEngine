#pragma once

#include "StdLib.h"
#include "Utility.h"

namespace BIEngine {

template <typename T, SizeT N>
class Array {
public:
   using ValueType = T;
   using Iterator = T*;
   using ConstIterator = const T*;

   SizeT Size() const { return N; }

   T& operator[](SizeT idx) { return m_pData[idx]; }

   const T& operator[](SizeT idx) const { return m_pData[idx]; }

   T* Data() { return &m_pData[0]; }

   const T* Data() const { return &m_pData[0]; }

   T& Front() { return m_pData[0]; }

   const T& Front() const { return m_pData[0]; }

   T& Back() { return m_pData[N - 1]; }

   const T& Back() const { return m_pData[N - 1]; }

   Iterator Begin() { return &m_pData[0]; }

   Iterator End() { return Begin() + N; }

   ConstIterator CBegin() const { return &m_pData[0]; }

   ConstIterator CEnd() const { return CBegin() + N; }

   void Fill(const T& val)
   {
      for (int i = 0; i < N; ++i) {
         m_pData[i] = val;
      }
   }

private:
   T m_pData[N];
};

template <typename T, SizeT N>
auto begin(Array<T, N>& arr)
{
   return arr.Begin();
}

template <typename T, SizeT N>
auto end(Array<T, N>& arr)
{
   return arr.End();
}

template <typename T, SizeT N>
auto begin(const Array<T, N>& arr)
{
   return arr.CBegin();
}

template <typename T, SizeT N>
auto end(const Array<T, N>& arr)
{
   return arr.CEnd();
}

template <typename T, SizeT N>
struct Hash<Array<T, N>> {
   SizeT operator()(const Array<T, N>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};

} // namespace BIEngine
