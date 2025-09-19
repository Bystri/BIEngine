#pragma once

#include "StdLib.h"
#include "Utility.h"
#include "Iterator.h"

#include <cstring>
#include <memory>
#include <initializer_list>

namespace BIEngine {

template <typename T>
class DynamicArray {
public:
   using ValueType = T;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstPointer = const ValueType*;
   using ConstReference = const ValueType&;
   using SizeType = SizeT;
   using Iterator = ValueType*;
   using ConstIterator = const ValueType*;
   using ReverseIterator = ReverseIterator<Iterator>;
   using ConstReverseIterator = ConstReverseIterator<ConstIterator>;

private:
   using Allocator = std::allocator<ValueType>;

public:
   DynamicArray() = default;
   explicit DynamicArray(SizeType n);
   DynamicArray(SizeType n, const ValueType& val);
   DynamicArray(std::initializer_list<ValueType> l);
   DynamicArray(const DynamicArray<T>& rhs);
   DynamicArray(DynamicArray<T>&& rhs);

   ~DynamicArray();

   DynamicArray<T>& operator=(const DynamicArray<T>& rhs);
   DynamicArray<T>& operator=(DynamicArray<T>&& rhs);

   SizeType Size() const;
   bool Empty() const;
   SizeType Capacity() const;

   Reference operator[](SizeT idx);
   ConstReference operator[](SizeT idx) const;

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

   void Reserve(SizeType newCap);
   void Resize(SizeType num);
   void Clear();

   Reference PushBack(const ValueType& val);
   Reference PushBack(ValueType&& val);

   template <typename... U>
   Reference EmplaceBack(U&&... args);

   void PopBack();

   Iterator Insert(const Iterator& pos, const ValueType& val);
   Iterator Insert(const Iterator& pos, ValueType&& val);

   Iterator Erase(const Iterator& pos);

private:
   void tryExpandDataStorage();
   void expandDataStorage(SizeType newCapacity);

   void free();

   template <typename DataType>
   typename DynamicArray<T>::Iterator insertImpl(const Iterator& pos, DataType&& val);

private:
   static constexpr SizeType GROW_RATIO = 2;

   ValueType* m_pBegin = nullptr;
   ValueType* m_pEnd = nullptr;
   SizeType m_capacity = 0;
};

/*DynamicArray*/

template <typename T>
DynamicArray<T>::DynamicArray(SizeType n)
{
   Reserve(n);
   for (int i = 0; i < n; ++i) {
      PushBack(ValueType());
   }
}

template <typename T>
DynamicArray<T>::DynamicArray(SizeType n, const ValueType& val)
{
   Reserve(n);
   for (int i = 0; i < n; ++i) {
      PushBack(val);
   }
}

template <typename T>
DynamicArray<T>::DynamicArray(std::initializer_list<ValueType> l)
{
   Reserve(l.size());
   for (const auto& val : l) {
      PushBack(val);
   }
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& rhs)
{
   Reserve(rhs.Size());
   for (int i = 0; i < rhs.Size(); ++i) {
      PushBack(rhs[i]);
   }
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& rhs)
{
   m_capacity = rhs.m_capacity;
   m_pBegin = rhs.m_pBegin;
   m_pEnd = rhs.m_pEnd;

   rhs.m_capacity = 0;
   rhs.m_pBegin = nullptr;
   rhs.m_pEnd = nullptr;
}

template <typename T>
DynamicArray<T>::~DynamicArray()
{
   free();
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& rhs)
{
   if (&rhs == this) {
      return *this;
   }

   Clear();
   Reserve(rhs.Size());
   for (int i = 0; i < rhs.Size(); ++i) {
      PushBack(rhs[i]);
   }

   return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& rhs)
{
   if (&rhs == this) {
      return *this;
   }

   free();

   m_capacity = rhs.m_capacity;
   m_pBegin = rhs.m_pBegin;
   m_pEnd = rhs.m_pEnd;

   rhs.m_capacity = 0;
   rhs.m_pBegin = nullptr;
   rhs.m_pEnd = nullptr;

   return *this;
}

template <typename T>
inline typename DynamicArray<T>::SizeType DynamicArray<T>::Size() const
{
   return m_pEnd - m_pBegin;
}

template <typename T>
inline bool DynamicArray<T>::Empty() const
{
   return m_pBegin == m_pEnd;
}

template <typename T>
inline typename DynamicArray<T>::SizeType DynamicArray<T>::Capacity() const
{
   return m_capacity;
}

template <typename T>
inline typename DynamicArray<T>::Reference DynamicArray<T>::operator[](SizeT idx)
{
   return *(m_pBegin + idx);
}

template <typename T>
inline typename DynamicArray<T>::ConstReference DynamicArray<T>::operator[](SizeT idx) const
{
   return *(m_pBegin + idx);
}

template <typename T>
inline typename DynamicArray<T>::Pointer DynamicArray<T>::Data()
{
   return m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::ConstPointer DynamicArray<T>::Data() const
{
   return m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::Reference DynamicArray<T>::Front()
{
   return *m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::ConstReference DynamicArray<T>::Front() const
{
   return *m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::Reference DynamicArray<T>::Back()
{
   return *(m_pEnd - 1);
}

template <typename T>
inline typename DynamicArray<T>::ConstReference DynamicArray<T>::Back() const
{
   return *(m_pEnd - 1);
}

template <typename T>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::Begin()
{
   return m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::End()
{
   return m_pEnd;
}

template <typename T>
inline typename DynamicArray<T>::ConstIterator DynamicArray<T>::Begin() const
{
   return m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::ConstIterator DynamicArray<T>::End() const
{
   return m_pEnd;
}

template <typename T>
inline typename DynamicArray<T>::ConstIterator DynamicArray<T>::CBegin() const
{
   return m_pBegin;
}

template <typename T>
inline typename DynamicArray<T>::ConstIterator DynamicArray<T>::CEnd() const
{
   return m_pEnd;
}

template <typename T>
inline typename DynamicArray<T>::ReverseIterator DynamicArray<T>::RBegin()
{
   return ReverseIterator(End());
}

template <typename T>
inline typename DynamicArray<T>::ReverseIterator DynamicArray<T>::REnd()
{
   return ReverseIterator(Begin());
}

template <typename T>
inline typename DynamicArray<T>::ConstReverseIterator DynamicArray<T>::RBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T>
inline typename DynamicArray<T>::ConstReverseIterator DynamicArray<T>::REnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T>
inline typename DynamicArray<T>::ConstReverseIterator DynamicArray<T>::CRBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T>
inline typename DynamicArray<T>::ConstReverseIterator DynamicArray<T>::CREnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T>
inline void DynamicArray<T>::Reserve(SizeType newCap)
{
   if (newCap <= m_capacity) {
      return;
   }

   expandDataStorage(newCap);
}

template <typename T>
inline void DynamicArray<T>::Resize(SizeType num)
{
   if (Size() == num) {
      return;
   }

   if (Size() < num) {
      Reserve(num);
      for (SizeType i = Size(); i < num; ++i) {
         EmplaceBack();
      }
      return;
   }

   SizeType cnt = Size() - num;
   while (cnt--) {
      PopBack();
   }
}

template <typename T>
inline void DynamicArray<T>::Clear()
{
   while (m_pEnd != m_pBegin) {
      PopBack();
   }
}

template <typename T>
inline typename DynamicArray<T>::Reference DynamicArray<T>::PushBack(const ValueType& val)
{
   tryExpandDataStorage();

   Allocator().construct(m_pEnd++, val);

   return Back();
}

template <typename T>
inline typename DynamicArray<T>::Reference DynamicArray<T>::PushBack(ValueType&& val)
{
   tryExpandDataStorage();

   Allocator().construct(m_pEnd++, std::move(val));

   return Back();
}

template <typename T>
template <typename... U>
inline typename DynamicArray<T>::Reference DynamicArray<T>::EmplaceBack(U&&... args)
{
   tryExpandDataStorage();

   Allocator().construct(m_pEnd++, std::forward<U>(args)...);

   return Back();
}

template <typename T>
inline void DynamicArray<T>::PopBack()
{
   Allocator().destroy(--m_pEnd);
}

template <typename T>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::Insert(const Iterator& pos, const ValueType& val)
{
   return insertImpl(pos, val);
}

template <typename T>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::Insert(const Iterator& pos, ValueType&& val)
{
   return insertImpl(pos, std::move(val));
}

template <typename T>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::Erase(const Iterator& pos)
{
   Allocator().destroy(pos);

   --m_pEnd;
   std::memmove(pos, pos + 1, (m_pEnd - pos) * sizeof(ValueType));

   return pos;
}

template <typename T>
template <typename DataType>
inline typename DynamicArray<T>::Iterator DynamicArray<T>::insertImpl(const Iterator& pos, DataType&& val)
{
   const SizeType idx = pos - m_pBegin;

   tryExpandDataStorage();

   std::memmove(&m_pBegin[idx + 1], &m_pBegin[idx], (Size() - idx) * sizeof(ValueType));
   Allocator().construct(m_pBegin + idx, std::forward<DataType>(val));
   ++m_pEnd;

   return m_pBegin + idx;
}

template <typename T>
inline void DynamicArray<T>::tryExpandDataStorage()
{
   if (Size() == m_capacity) {
      expandDataStorage(m_capacity == 0 ? 1 : m_capacity * GROW_RATIO);
   }
}

template <typename T>
inline void DynamicArray<T>::expandDataStorage(SizeType newCapacity)
{
   ValueType* newData = Allocator().allocate(newCapacity);

   std::memcpy(newData, m_pBegin, m_capacity * sizeof(T));

   m_pEnd = newData + Size();
   Allocator().deallocate(m_pBegin, m_capacity);
   m_pBegin = newData;
   m_capacity = newCapacity;
}

template <typename T>
inline void DynamicArray<T>::free()
{
   Clear();

   Allocator().deallocate(m_pBegin, m_capacity);

   m_pBegin = nullptr;
   m_pEnd = nullptr;
   m_capacity = 0;
}

/*foreach loop helpers*/

template <typename T>
auto begin(DynamicArray<T>& arr)
{
   return arr.Begin();
}

template <typename T>
auto end(DynamicArray<T>& arr)
{
   return arr.End();
}

template <typename T>
auto begin(const DynamicArray<T>& arr)
{
   return arr.CBegin();
}

template <typename T>
auto end(const DynamicArray<T>& arr)
{
   return arr.CEnd();
}

/*Hash*/

template <typename T>
struct Hash<DynamicArray<T>> {
   SizeT operator()(const DynamicArray<T>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};


} // namespace BIEngine
