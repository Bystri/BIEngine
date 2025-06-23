#pragma once

#include "StdLib.h"

namespace BIEngine {

template <typename T, SizeT N>
class Array {
public:
   using Iterator = T*;
   using ConstIterator = const T*;

   SizeT Size() const { return N; }

   T& operator[](SizeT idx) { return data[idx]; }

   const T& operator[](SizeT idx) const { return data[idx]; }

   T* Data() { return &data[0]; }

   const T* Data() const { return &data[0]; }

   T& Front() { return data[0]; }

   const T& Front() const { return data[0]; }

   T& Back() { return data[N - 1]; }

   const T& Back() const { return data[N - 1]; }

   Iterator Begin() { return &data[0]; }

   Iterator End() { return Begin() + N; }

   ConstIterator CBegin() const { return &data[0]; }

   ConstIterator CEnd() const { return CBegin() + N; }

   void Fill(const T& val)
   {
      for (int i = 0; i < N; ++i) {
         data[i] = val;
      }
   }

private:
   T data[N];
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

} // namespace BIEngine
