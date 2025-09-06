#pragma once

#include "StdLib.h"
#include "Utility.h"
#include "Iterator.h"

namespace BIEngine {

template <typename T, SizeT N>
class Array {
public:
   using ValueType = T;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstPointer = const ValueType*;
   using ConstReference = const ValueType&;
   using SizeType = SizeT;
   using Iterator = T*;
   using ConstIterator = const T*;
   using ReverseIterator = ReverseIterator<Iterator>;
   using ConstReverseIterator = ConstReverseIterator<ConstIterator>;

   SizeType Size() const;
   bool Empty() const;

   Reference operator[](SizeType idx);
   ConstReference operator[](SizeType idx) const;

   Pointer Data();
   ConstPointer Data() const;

   Reference Front();
   ConstReference Front() const;

   Reference Back();
   ConstReference Back() const;

   Iterator Begin();
   Iterator End();

   ConstIterator Begin() const;
   ConstIterator End() const;

   ConstIterator CBegin() const;
   ConstIterator CEnd() const;

   ReverseIterator RBegin();
   ReverseIterator REnd();

   ConstReverseIterator RBegin() const;
   ConstReverseIterator REnd() const;

   ConstReverseIterator CRBegin() const;
   ConstReverseIterator CREnd() const;

   void Fill(const ValueType& val);

private:
   ValueType m_pData[N];
};

/*Array*/

template <typename T, SizeT N>
inline typename Array<T, N>::SizeType Array<T, N>::Size() const
{
   return (SizeType)N;
}

template <typename T, SizeT N>
inline bool Array<T, N>::Empty() const
{
   return N == 0;
}

template <typename T, SizeT N>
inline typename Array<T, N>::Reference Array<T, N>::operator[](SizeType idx)
{
   return m_pData[idx];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReference Array<T, N>::operator[](SizeType idx) const
{
   return m_pData[idx];
}

template <typename T, SizeT N>
inline typename Array<T, N>::Pointer Array<T, N>::Data()
{
   return m_pData;
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstPointer Array<T, N>::Data() const
{
   return m_pData;
}

template <typename T, SizeT N>
inline typename Array<T, N>::Reference Array<T, N>::Front()
{
   return m_pData[0];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReference Array<T, N>::Front() const
{
   return m_pData[0];
}

template <typename T, SizeT N>
inline typename Array<T, N>::Reference Array<T, N>::Back()
{
   return m_pData[N - 1];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReference Array<T, N>::Back() const
{
   return m_pData[N - 1];
}

template <typename T, SizeT N>
inline typename Array<T, N>::Iterator Array<T, N>::Begin()
{
   return &m_pData[0];
}

template <typename T, SizeT N>
inline typename Array<T, N>::Iterator Array<T, N>::End()
{
   return &m_pData[N];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstIterator Array<T, N>::Begin() const
{
   return &m_pData[0];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstIterator Array<T, N>::End() const
{
   return &m_pData[N];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstIterator Array<T, N>::CBegin() const
{
   return &m_pData[0];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstIterator Array<T, N>::CEnd() const
{
   return &m_pData[N];
}

template <typename T, SizeT N>
inline typename Array<T, N>::ReverseIterator Array<T, N>::RBegin()
{
   return ReverseIterator(End());
}

template <typename T, SizeT N>
inline typename Array<T, N>::ReverseIterator Array<T, N>::REnd()
{
   return ReverseIterator(Begin());
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReverseIterator Array<T, N>::RBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReverseIterator Array<T, N>::REnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReverseIterator Array<T, N>::CRBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T, SizeT N>
inline typename Array<T, N>::ConstReverseIterator Array<T, N>::CREnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T, SizeT N>
void Array<T, N>::Fill(const ValueType& val)
{
   for (int i = 0; i < N; ++i) {
      m_pData[i] = val;
   }
}

/*foreach loop helpers*/

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

/*Hash*/

template <typename T, SizeT N>
struct Hash<Array<T, N>> {
   SizeT operator()(const Array<T, N>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};

} // namespace BIEngine
