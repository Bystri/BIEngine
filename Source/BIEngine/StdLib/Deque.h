#pragma once

#include <memory>

#include "StdLib.h"
#include "Algorithm.h"
#include "DynamicArray.h"

namespace BIEngine {

    #pragma optimize("",off)

template <class T>
class Deque {
   class ConstDequeIterator : public IteratorBase<BiderectinalIteratorTag, T> {
      friend class Deque;

   public:
      ConstDequeIterator& operator=(const ConstDequeIterator& rhs)
      {
         m_pDeque = rhs.m_pDeque;
         m_curIdx = rhs.m_curIdx;

         return *this;
      }

      ConstDequeIterator& operator++()
      {
         m_curIdx = (m_curIdx == m_pDeque->m_firstIdx + m_pDeque->m_size ? m_pDeque->m_firstIdx : m_curIdx + 1);
         return *this;
      }

      ConstDequeIterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      ConstDequeIterator& operator--()
      {
         m_curIdx = (m_curIdx == m_pDeque->m_firstIdx ? m_pDeque->m_firstIdx + m_pDeque->m_size : m_curIdx - 1);
         return *this;
      }

      ConstDequeIterator operator--(int)
      {
         Iterator old = *this;
         operator--();
         return old;
      }

      const typename IteratorTraits<ConstDequeIterator>::Reference operator*() const
      {
         const SizeT mapIdx = m_pDeque->getMapIdxFromDequeIdx(m_curIdx);
         const SizeT blockIdx = m_pDeque->getBlockIdxFromDequeIdx(m_curIdx);
         return m_pDeque->m_chunkMap[mapIdx][blockIdx];
      }

      const typename IteratorTraits<ConstDequeIterator>::Pointer operator->() const
      {
         const SizeT mapIdx = m_pDeque->getMapIdxFromDequeIdx(m_curIdx);
         const SizeT blockIdx = m_pDeque->getBlockIdxFromDequeIdx(m_curIdx);
         return &m_pDeque->m_chunkMap[mapIdx][blockIdx];
      }

      friend bool operator==(const ConstDequeIterator& lhs, const ConstDequeIterator& rhs)
      {
         return lhs.m_curIdx == rhs.m_curIdx;
      }

      friend bool operator!=(const ConstDequeIterator& lhs, const ConstDequeIterator& rhs)
      {
         return !(lhs == rhs);
      }

   protected:
      ConstDequeIterator(const Deque<T>* pDeque, SizeT curIdx)
         : m_pDeque(pDeque), m_curIdx(curIdx)
      {
      }

   protected:
      SizeT m_curIdx;
      const Deque<T>* m_pDeque;
   };

   class DequeIterator : public ConstDequeIterator {
      friend class Deque;

   public:
      const typename IteratorTraits<DequeIterator>::Reference operator*() const
      {
         const SizeT mapIdx = this->m_pDeque->getMapIdxFromDequeIdx(this->m_curIdx);
         const SizeT blockIdx = this->m_pDeque->getBlockIdxFromDequeIdx(this->m_curIdx);
         return this->m_pDeque->m_chunkMap[mapIdx][blockIdx];
      }

      const typename IteratorTraits<DequeIterator>::Pointer operator->() const
      {
         const SizeT mapIdx = this->m_pDeque->getMapIdxFromDequeIdx(this->m_curIdx);
         const SizeT blockIdx = this->m_pDeque->getBlockIdxFromDequeIdx(this->m_curIdx);
         return &this->m_pDeque->m_chunkMap[mapIdx][blockIdx];
      }

   protected:
      DequeIterator(const Deque<T>* pDeque, SizeT curIdx)
         : ConstDequeIterator(pDeque, curIdx)
      {
      }
   };

public:
   using ValueType = T;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstPointer = const ValueType*;
   using ConstReference = const ValueType&;
   using SizeType = SizeT;
   using Iterator = DequeIterator;
   using ConstIterator = ConstDequeIterator;
   using ReverseIterator = ReverseIterator<Iterator>;
   using ConstReverseIterator = ConstReverseIterator<ConstIterator>;

private:
   using DataAllocator = std::allocator<ValueType>;

public:
   Deque();
   Deque(SizeType count);
   Deque(SizeType count, const ValueType& val);
   Deque(const Deque<T>& other);
   Deque(Deque<T>&& other);

   ~Deque();

   Deque<T>& operator=(const Deque<T>& other);
   Deque<T>& operator=(Deque<T>&& other);

   Reference operator[](SizeType idx);
   ConstReference operator[](SizeType idx) const;

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

   SizeType Size() const;
   bool Empty() const;
   void Clear();

   void PushBack(const ValueType& val);
   void PushBack(ValueType&& val);

   template <typename... Args>
   void EmplaceBack(Args&&... args);

   void PushFront(const ValueType& val);
   void PushFront(ValueType&& val);

   template <typename... Args>
   void EmplaceFront(Args&&... args);

   void PopBack();
   void PopFront();

   void Resize(SizeType num);

private:
   void free();

   SizeType getMapIdxFromDequeIdx(SizeType idx) const;
   SizeType getBlockIdxFromDequeIdx(SizeType idx) const;

   void createInitialMap(SizeType n);
   void expandChunkMap(SizeType chunksNum);

   void tryGrowFront();
   void tryGrowBack();

   template <typename DataType>
   void pushBackImpl(DataType&& val);

   template <typename DataType>
   void pushFrontImpl(DataType&& val);

private:
   static constexpr SizeType CHUNK_SIZE = 16;

   SizeType m_firstIdx;
   SizeType m_size = 0;
   DynamicArray<ValueType*> m_chunkMap;
};

/*Deque*/

template <typename T>
inline Deque<T>::Deque()
{
   createInitialMap(0);
}

template <typename T>
inline Deque<T>::Deque(SizeType count)
{
   createInitialMap(count);
   while (count--) {
      EmplaceBack();
   }
}

template <typename T>
inline Deque<T>::Deque(SizeType count, const ValueType& val)
{
   createInitialMap(count);
   while (count--) {
      PushBack(val);
   }
}

template <typename T>
inline Deque<T>::Deque(const Deque<T>& other)
{
   createInitialMap(other.Size());
   auto end = other.CEnd();
   for (auto itr = other.CBegin(); itr != end; ++itr) {
      PushBack(*itr);
   }
}

template <typename T>
inline Deque<T>::Deque(Deque<T>&& other)
   : m_chunkMap(std::move(other.m_chunkMap)), m_firstIdx(other.m_firstIdx), m_size(other.m_size)
{
   other.m_firstIdx = 0;
   other.m_size = 0;
}

template <typename T>
inline Deque<T>::~Deque()
{
   free();
}

template <typename T>
inline Deque<T>& Deque<T>::operator=(const Deque<T>& other)
{
   if (this == &other) {
      return *this;
   }

   Clear();

   auto end = other.CEnd();
   for (auto itr = other.CBegin(); itr != end; ++itr) {
      PushBack(*itr);
   }

   return *this;
}

template <typename T>
inline Deque<T>& Deque<T>::operator=(Deque<T>&& other)
{
   if (this == &other) {
      return *this;
   }

   free();

   m_chunkMap = std::move(other.m_chunkMap);
   m_firstIdx = other.m_firstIdx;
   m_size = other.m_size;

   other.m_firstIdx = 0;
   other.m_size = 0;

   return *this;
}

template <typename T>
inline typename Deque<T>::Reference Deque<T>::operator[](SizeType idx)
{
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + idx);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + idx);
   return m_chunkMap[mapIdx][blockIdx];
}

template <typename T>
inline typename Deque<T>::ConstReference Deque<T>::operator[](SizeType idx) const
{
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + idx);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + idx);
   return m_chunkMap[mapIdx][blockIdx];
}

template <typename T>
inline typename Deque<T>::Reference Deque<T>::Front()
{
   return this->operator[](0);
}

template <typename T>
inline typename Deque<T>::ConstReference Deque<T>::Front() const
{
   return this->operator[](0);
}

template <typename T>
inline typename Deque<T>::Reference Deque<T>::Back()
{
   return this->operator[](Size() - 1);
}

template <typename T>
inline typename Deque<T>::ConstReference Deque<T>::Back() const
{
   return this->operator[](Size() - 1);
}

template <typename T>
inline typename Deque<T>::Iterator Deque<T>::Begin()
{
   return Iterator(this, m_firstIdx);
}

template <typename T>
inline typename Deque<T>::Iterator Deque<T>::End()
{
   return Iterator(this, m_firstIdx + m_size);
}

template <typename T>
inline typename Deque<T>::ConstIterator Deque<T>::Begin() const
{
   return ConstIterator(this, m_firstIdx);
}

template <typename T>
inline typename Deque<T>::ConstIterator Deque<T>::End() const
{
   return ConstIterator(this, m_firstIdx + m_size);
}

template <typename T>
inline typename Deque<T>::ConstIterator Deque<T>::CBegin() const
{
   return ConstIterator(this, m_firstIdx);
}

template <typename T>
inline typename Deque<T>::ConstIterator Deque<T>::CEnd() const
{
   return ConstIterator(this, m_firstIdx + m_size);
}

template <typename T>
inline typename Deque<T>::ReverseIterator Deque<T>::RBegin()
{
   return ReverseIterator(End());
}

template <typename T>
inline typename Deque<T>::ReverseIterator Deque<T>::REnd()
{
   return ReverseIterator(Begin());
}

template <typename T>
inline typename Deque<T>::ConstReverseIterator Deque<T>::RBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T>
inline typename Deque<T>::ConstReverseIterator Deque<T>::REnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T>
inline typename Deque<T>::ConstReverseIterator Deque<T>::CRBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename T>
inline typename Deque<T>::ConstReverseIterator Deque<T>::CREnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename T>
inline typename Deque<T>::SizeType Deque<T>::Size() const
{
   return m_size;
}

template <typename T>
inline bool Deque<T>::Empty() const
{
   return m_size == 0;
}

template <typename T>
inline void Deque<T>::Clear()
{
   SizeType cnt = Size();

   while (cnt--) {
      PopBack();
   }
}

template <typename T>
inline void Deque<T>::PushBack(const ValueType& val)
{
   pushBackImpl(val);
}

template <typename T>
inline void Deque<T>::PushBack(ValueType&& val)
{
   pushBackImpl(std::move(val));
}

template <typename T>
template <typename... Args>
inline void Deque<T>::EmplaceBack(Args&&... args)
{
   tryGrowBack();

   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + m_size);
   DataAllocator().construct(m_chunkMap[mapIdx] + blockIdx, std::forward<Args>(args)...);
   ++m_size;
}

template <typename T>
inline void Deque<T>::PushFront(const ValueType& val)
{
   pushFrontImpl(val);
}

template <typename T>
inline void Deque<T>::PushFront(ValueType&& val)
{
   pushFrontImpl(std::move(val));
}

template <typename T>
template <typename... Args>
inline void Deque<T>::EmplaceFront(Args&&... args)
{
   tryGrowFront();

   --m_firstIdx;
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx);
   DataAllocator().construct(m_chunkMap[mapIdx] + blockIdx, std::forward<Args>(args)...);
   ++m_size;
}

template <typename T>
inline void Deque<T>::PopBack()
{
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size - 1);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + m_size - 1);
   DataAllocator().destroy(m_chunkMap[mapIdx] + blockIdx);

   --m_size;

   if (blockIdx == 0) {
      DataAllocator().deallocate(m_chunkMap[mapIdx], CHUNK_SIZE);
   }
}

template <typename T>
inline void Deque<T>::PopFront()
{
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx);
   DataAllocator().destroy(m_chunkMap[mapIdx] + blockIdx);

   ++m_firstIdx;
   --m_size;

   if (blockIdx == CHUNK_SIZE - 1) {
      DataAllocator().deallocate(m_chunkMap[mapIdx], CHUNK_SIZE);
   }
}

template <typename T>
inline void Deque<T>::Resize(SizeType num)
{
   if (num == Size()) {
      return;
   }

   if (Size() < num) {
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
inline void Deque<T>::free()
{
   const SizeType currentFirstChunkIdx = getMapIdxFromDequeIdx(m_firstIdx);

   Clear();
}

template <typename T>
inline typename Deque<T>::SizeType Deque<T>::getMapIdxFromDequeIdx(SizeType idx) const
{
   return idx / CHUNK_SIZE;
}

template <typename T>
inline typename Deque<T>::SizeType Deque<T>::getBlockIdxFromDequeIdx(SizeType idx) const
{
   return idx - getMapIdxFromDequeIdx(idx) * CHUNK_SIZE;
}

template <typename T>
inline void Deque<T>::createInitialMap(SizeType n)
{
   const SizeType numNodes = n / CHUNK_SIZE + 1;

   SizeType mapSize = Max(static_cast<SizeType>(8), numNodes + 2);
   m_chunkMap.Resize(mapSize);

   m_firstIdx = mapSize / 2 * CHUNK_SIZE;
}

template <typename T>
inline void Deque<T>::expandChunkMap(SizeType chunksNum)
{
   DynamicArray<ValueType*> newChunkMap(chunksNum);

   const SizeType firstBlockIdx = getBlockIdxFromDequeIdx(m_firstIdx);

   const SizeType currentFirstChunkIdx = getMapIdxFromDequeIdx(m_firstIdx);
   const SizeType currentLastChunkIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size);
   const SizeType currentFilledChunksNum = currentLastChunkIdx - currentFirstChunkIdx + 1;
   const SizeType newFirstChunckIdx = (chunksNum - currentFilledChunksNum) / 2;

   for (int i = 0; i < currentFilledChunksNum; ++i) {
      newChunkMap[newFirstChunckIdx + i] = m_chunkMap[currentFirstChunkIdx + i];
   }

   m_chunkMap = newChunkMap;

   m_firstIdx = newFirstChunckIdx * CHUNK_SIZE + firstBlockIdx;
}

template <typename T>
inline void Deque<T>::tryGrowFront()
{
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx);
   if (blockIdx != 0) {
      return;
   }

   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx);
   if (mapIdx > 0) {
      m_chunkMap[mapIdx - 1] = DataAllocator().allocate(CHUNK_SIZE);
      return;
   }

   expandChunkMap(m_chunkMap.Size() * 2);
   const SizeType newMapIdx = getMapIdxFromDequeIdx(m_firstIdx);
   m_chunkMap[newMapIdx - 1] = DataAllocator().allocate(CHUNK_SIZE);
}

template <typename T>
inline void Deque<T>::tryGrowBack()
{
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + m_size);
   if (blockIdx != 0) {
      return;
   }

   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size);
   if (mapIdx < m_chunkMap.Size() - 1) {
      m_chunkMap[mapIdx] = DataAllocator().allocate(CHUNK_SIZE);
      return;
   }

   expandChunkMap(m_chunkMap.Size() * 2);
   const SizeType newMapIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size);
   m_chunkMap[newMapIdx] = DataAllocator().allocate(CHUNK_SIZE);
}

template <typename T>
template <typename DataType>
inline void Deque<T>::pushBackImpl(DataType&& val)
{
   tryGrowBack();

   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx + m_size);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx + m_size);
   DataAllocator().construct(m_chunkMap[mapIdx] + blockIdx, std::forward<DataType>(val));
   ++m_size;
}

template <typename T>
template <typename DataType>
inline void Deque<T>::pushFrontImpl(DataType&& val)
{
   tryGrowFront();

   --m_firstIdx;
   const SizeType mapIdx = getMapIdxFromDequeIdx(m_firstIdx);
   const SizeType blockIdx = getBlockIdxFromDequeIdx(m_firstIdx);
   DataAllocator().construct(m_chunkMap[mapIdx] + blockIdx, std::forward<DataType>(val));
   ++m_size;
}

/*foreach loop helpers*/

template <typename T>
auto begin(Deque<T>& deque)
{
   return deque.Begin();
}

template <typename T>
auto end(Deque<T>& deque)
{
   return deque.End();
}

template <typename T>
auto begin(const Deque<T>& deque)
{
   return deque.CBegin();
}

template <typename T>
auto end(const Deque<T>& deque)
{
   return deque.CEnd();
}

} // namespace BIEngine
