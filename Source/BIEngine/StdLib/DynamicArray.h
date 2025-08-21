#pragma once

#include "StdLib.h"
#include "Utility.h"

#include <cstring>
#include <memory>
#include <initializer_list>

namespace BIEngine {

template <typename T>
class DynamicArray {
public:
   using ValueType = T;
   using Iterator = T*;
   using ConstIterator = const T*;

   DynamicArray() = default;

   explicit DynamicArray(SizeT n)
   {
      Reserve(n);
      for (int i = 0; i < n; ++i) {
         PushBack(T());
      }
   }

   DynamicArray(SizeT n, const T& val)
   {
      Reserve(n);
      for (int i = 0; i < n; ++i) {
         PushBack(val);
      }
   }

   DynamicArray(std::initializer_list<T> l)
   {
      Reserve(l.size());
      for (const auto& val : l) {
         PushBack(val);
      }
   }

   DynamicArray(const DynamicArray<T>& rhs)
   {
      Reserve(rhs.Size());
      for (int i = 0; i < rhs.Size(); ++i) {
         PushBack(rhs[i]);
      }
   }

   DynamicArray(DynamicArray<T>&& rhs)
   {
      m_capacity = rhs.m_capacity;
      m_pBegin = rhs.m_pBegin;
      m_pEnd = rhs.m_pEnd;

      rhs.m_capacity = 0;
      rhs.m_pBegin = nullptr;
      rhs.m_pEnd = nullptr;
   }

   DynamicArray<T>& operator=(const DynamicArray<T>& rhs)
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

   DynamicArray<T>& operator=(DynamicArray<T>&& rhs)
   {
      if (&rhs == this) {
         return *this;
      }

      this->~DynamicArray();

      m_capacity = rhs.m_capacity;
      m_pBegin = rhs.m_pBegin;
      m_pEnd = rhs.m_pEnd;

      rhs.m_capacity = 0;
      rhs.m_pBegin = nullptr;
      rhs.m_pEnd = nullptr;

      return *this;
   }

   ~DynamicArray()
   {
      Clear();

      m_allocator.deallocate(m_pBegin, m_capacity);
   }

   T& operator[](SizeT idx)
   {
      return *(m_pBegin + idx);
   }

   const T& operator[](SizeT idx) const { return *(m_pBegin + idx); }

   T& Front() { return *m_pBegin; }

   const T& Front() const { return *m_pBegin; }

   T& Back() { return *(m_pEnd - 1); }

   const T& Back() const { return *(m_pEnd - 1); }

   T* Data() { return m_pBegin; }

   T* Data() const { return m_pBegin; }

   Iterator Begin() { return m_pBegin; }

   Iterator End() { return m_pEnd; }

   ConstIterator CBegin() const { return m_pBegin; }

   ConstIterator CEnd() const { return m_pEnd; }

   bool Empty() const { return m_pBegin == m_pEnd; }

   SizeT Size() const
   {
      return m_pEnd - m_pBegin;
   }

   SizeT Capacity() const { return m_capacity; }

   void Reserve(SizeT newCap)
   {
      if (newCap <= m_capacity) {
         return;
      }

      expandDataStorage(newCap);
   }

   T& PushBack(const T& val)
   {
      tryExpandDataStorage();

      m_allocator.construct(m_pEnd++, val);

      return Back();
   }

   T& PushBack(T&& val)
   {
      tryExpandDataStorage();

      m_allocator.construct(m_pEnd++, std::move(val));

      return Back();
   }

   template <typename... U>
   T& EmplaceBack(U&&... args)
   {
      tryExpandDataStorage();

      m_allocator.construct(m_pEnd++, std::forward<U>(args)...);

      return Back();
   }

   void Resize(int num)
   {
      if (Size() == num) {
         return;
      }

      if (Size() < num) {
         Reserve(num);
         for (int i = Size(); i < num; ++i) {
            EmplaceBack();
         }
         return;
      }

      if (Size() > num) {
         int cnt = Size() - num;
         while (cnt--) {
            PopBack();
         }
      }
   }

   void PopBack()
   {
      m_allocator.destroy(--m_pEnd);
   }

   void Clear()
   {
      while (m_pEnd != m_pBegin) {
         PopBack();
      }
   }

   Iterator Insert(const Iterator& pos, const T& val)
   {
      const SizeT idx = pos - m_pBegin;

      tryExpandDataStorage();

      std::memmove(&m_pBegin[idx + 1], &m_pBegin[idx], (Size() - idx) * sizeof(T));
      m_allocator.construct(m_pBegin + idx, val);

      return m_pBegin + idx;
   }

   Iterator Insert(const Iterator& pos, T&& val)
   {
      const SizeT idx = pos - m_pBegin;

      tryExpandDataStorage();

      std::memmove(&m_pBegin[idx + 1], &m_pBegin[idx], (Size() - idx) * sizeof(T));
      m_allocator.construct(m_pBegin + idx, std::move(val));

      return m_pBegin + idx;
   }

   Iterator Erase(const Iterator& pos)
   {
      m_allocator.destroy(pos);

      --m_pEnd;
      std::memmove(pos, pos + 1, (m_pEnd - pos) * sizeof(T));

      return pos;
   }


private:
   void tryExpandDataStorage()
   {
      if (Size() == m_capacity) {
         expandDataStorage(m_capacity == 0 ? 1 : m_capacity * GROW_RATIO);
      }
   }

   void expandDataStorage(SizeT newCapacity)
   {
      T* newData = m_allocator.allocate(newCapacity);

      std::memcpy(newData, m_pBegin, m_capacity * sizeof(T));

      m_pEnd = newData + Size();
      m_allocator.deallocate(m_pBegin, m_capacity);
      m_pBegin = newData;
      m_capacity = newCapacity;
   }

private:
   static constexpr SizeT GROW_RATIO = 2;

   T* m_pBegin = nullptr;
   T* m_pEnd = nullptr;
   SizeT m_capacity = 0;
   std::allocator<T> m_allocator;
};

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

template <typename T>
struct Hash<DynamicArray<T>> {
   SizeT operator()(const DynamicArray<T>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};


} // namespace BIEngine
