#pragma once

#include <functional>

#include "DynamicArray.h"
#include "ForwardList.h"

namespace BIEngine {

template <typename Key, typename T>
class HashMap {
public:
   class Iterator {
      friend class HashMap;

   public:
      Iterator& operator=(const Iterator& rhs)
      {
         m_pHashMap = rhs.m_pHashMap;
         m_bucketIdx = rhs.m_bucketIdx;
         m_listItr = rhs.m_listItr;

         return *this;
      }

      Iterator& operator++()
      {
         ++m_listItr;
         if (m_pHashMap->m_storage[m_bucketIdx].End() == m_listItr) {
            while (++m_bucketIdx < m_pHashMap->m_storage.Size() && m_pHashMap->m_storage[m_bucketIdx].Empty()) {
            }

            if (m_bucketIdx >= m_pHashMap->m_storage.Size()) {
               m_listItr = m_pHashMap->m_storage[0].End();
            } else {
               m_listItr = m_pHashMap->m_storage[m_bucketIdx].Begin();
            }
         }

         return *this;
      }

      Iterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      std::pair<const Key, T>& operator*()
      {
         return *m_listItr;
      }

      std::pair<const Key, T>* operator->()
      {
         return m_listItr.operator->();
      }

      friend bool operator==(const Iterator& lhs, const Iterator& rhs)
      {
         return lhs.m_bucketIdx == rhs.m_bucketIdx && lhs.m_listItr == rhs.m_listItr;
      }

      friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
      {
         return !(lhs == rhs);
      }

   private:
      Iterator(HashMap<Key, T>* pHashMap, SizeT bucketIdx, typename ForwardList<std::pair<const Key, T>>::Iterator listItr)
         : m_pHashMap(pHashMap), m_bucketIdx(bucketIdx), m_listItr(listItr)
      {
      }

   private:
      HashMap<Key, T>* m_pHashMap;
      SizeT m_bucketIdx;
      typename ForwardList<std::pair<const Key, T>>::Iterator m_listItr;
   };

   class ConstIterator {
      friend class HashMap;

   public:
      ConstIterator& operator=(const ConstIterator& rhs)
      {
         m_pHashMap = rhs.m_pHashMap;
         m_bucketIdx = rhs.m_bucketIdx;
         m_listItr = rhs.m_listItr;

         return *this;
      }

      ConstIterator& operator++()
      {
         ++m_listItr;
         if (m_pHashMap->m_storage[m_bucketIdx].CEnd() == m_listItr) {
            while (++m_bucketIdx < m_pHashMap->m_storage.Size() && m_pHashMap->m_storage[m_bucketIdx].Empty()) {
            }

            if (m_bucketIdx >= m_pHashMap->m_storage.Size()) {
               m_listItr = m_pHashMap->m_storage[0].CEnd();
            } else {
               m_listItr = m_pHashMap->m_storage[m_bucketIdx].CBegin();
            }
         }

         return *this;
      }

      ConstIterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      const std::pair<const Key, T>& operator*() const
      {
         return *m_listItr;
      }

      const std::pair<const Key, T>* operator->() const
      {
         return m_listItr.operator->();
      }

      friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs)
      {
         return lhs.m_bucketIdx == rhs.m_bucketIdx && lhs.m_listItr == rhs.m_listItr;
      }

      friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs)
      {
         return !(lhs == rhs);
      }

   private:
      ConstIterator(const HashMap<Key, T>* pHashMap, SizeT bucketIdx, typename ForwardList<std::pair<const Key, T>>::ConstIterator listItr)
         : m_pHashMap(pHashMap), m_bucketIdx(bucketIdx), m_listItr(listItr)
      {
      }

   private:
      const HashMap<Key, T>* m_pHashMap;
      SizeT m_bucketIdx;
      typename ForwardList<std::pair<const Key, T>>::ConstIterator m_listItr;
   };

   HashMap()
   {
      m_storage.Resize(8);
   }

   SizeT Size() const
   {
      return m_size;
   }

   bool Empty() const
   {
      return m_size == 0;
   }

   Iterator Begin()
   {
      if (m_size == 0) {
         return End();
      }

      int bucketIdx = -1;
      while (m_storage[++bucketIdx].Empty()) {
      }

      return Iterator(this, bucketIdx, m_storage[bucketIdx].Begin());
   }

   Iterator End()
   {
      return Iterator(this, m_storage.Size(), m_storage[0].End());
   }

   ConstIterator CBegin() const
   {
      if (m_size == 0) {
         return CEnd();
      }

      int bucketIdx = -1;
      while (m_storage[++bucketIdx].Empty()) {
      }

      return ConstIterator(this, bucketIdx, m_storage[bucketIdx].CBegin());
   }

   ConstIterator CEnd() const
   {
      return ConstIterator(this, m_storage.Size(), m_storage[0].CEnd());
   }

   T& operator[](const Key& key)
   {
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End(); ++itr) {
         if (itr->first == key) {
            return itr->second;
         }
      }

      return Emplace(key, T{}).first->second;
   }

   Iterator Find(const Key& key)
   {
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End(); ++itr) {
         if (itr->first == key) {
            return Iterator(this, bucketIdx, itr);
         }
      }

      return End();
   }

   ConstIterator Find(const Key& key) const
   {
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
         if (itr->first == key) {
            return ConstIterator(this, bucketIdx, itr);
         }
      }

      return CEnd();
   }

   void Clear()
   {
      for (auto itr = m_storage.Begin(); itr != m_storage.End(); ++itr) {
         itr->Clear();
      }

      m_size = 0;
   }

   std::pair<Iterator, bool> Insert(const Key& key, const T& val)
   {
      Iterator itr = Find(key);
      if (itr != End()) {
         return std::pair<Iterator, bool>(itr, false);
      }

      tryRehash();

      const SizeT bucketIdx = getBucketIdxFromKey(key);

      m_storage[bucketIdx].PushFront({key, val});
      ++m_size;

      return std::pair<Iterator, bool>(Iterator(this, bucketIdx, m_storage[bucketIdx].Begin()), true);
   }

   template <typename... U>
   std::pair<Iterator, bool> Emplace(const Key& key, U&&... args)
   {
      Iterator itr = Find(key);
      if (itr != End()) {
         return std::pair<Iterator, bool>(itr, false);
      }

      tryRehash();

      const SizeT bucketIdx = getBucketIdxFromKey(key);

      m_storage[bucketIdx].EmplaceFront(key, std::forward<U>(args)...);
      ++m_size;

      return std::pair<Iterator, bool>(Iterator(this, bucketIdx, m_storage[bucketIdx].Begin()), true);
   }

   SizeT Erase(const Key& key)
   {
      SizeT retVal = 0;
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      auto itrBefore = m_storage[bucketIdx].BeforeBegin();

      for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
         if (itr->first == key) {
            itr = m_storage[bucketIdx].EraseAfter(itrBefore);
            ++retVal;
         } else {
            ++itr;
            ++itrBefore;
         }
      }

      return retVal;
   }

   Iterator Erase(Iterator itrToErase)
   {
      auto itrBefore = m_storage[itrToErase.m_bucketIdx].BeforeBegin();
      auto itr = m_storage[itrToErase.m_bucketIdx].Begin();
      while (itr != itrToErase.m_listItr) {
         ++itr;
         ++itrBefore;
      }

      m_storage[itrToErase.m_bucketIdx].EraseAfter(itrBefore);
      auto retItr = Iterator(this, itrToErase.m_bucketIdx, itrBefore);
      ++retItr;

      return retItr;
   }

   float LoadFactor() const
   {
      return static_cast<float>(Size()) / m_storage.Size();
   }

   float MaxLoadFactor() const
   {
      return 0.8f;
   }

private:
   SizeT getBucketIdxFromKey(const Key& key) const
   {
      const SizeT hash = std::hash<Key>()(key);
      return hash % m_storage.Size();
   }

   void tryRehash()
   {
      if (MaxLoadFactor() > LoadFactor()) {
         return;
      }

      rehash(m_storage.Size() * 2);
   }

   void rehash(SizeT n)
   {
      DynamicArray<ForwardList<std::pair<const Key, T>>> newStorage(n);

      for (int bucketIdx = 0; bucketIdx < m_storage.Size(); ++bucketIdx) {
         for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
            const SizeT hash = std::hash<Key>()(itr->first);
            SizeT newIdx = hash % newStorage.Size();

            ++itr;
            newStorage[newIdx].SpliceAfter(newStorage[newIdx].BeforeBegin(), m_storage[bucketIdx], m_storage[bucketIdx].BeforeBegin());
         }
      }

      m_storage = std::move(newStorage);
   }

private:
   SizeT m_size = 0;
   DynamicArray<ForwardList<std::pair<const Key, T>>> m_storage;
};

template <typename Key, typename T>
auto begin(HashMap<Key, T>& map)
{
   return map.Begin();
}

template <typename Key, typename T>
auto end(HashMap<Key, T>& map)
{
   return map.End();
}

template <typename Key, typename T>
auto begin(const HashMap<Key, T>& map)
{
   return map.CBegin();
}

template <typename Key, typename T>
auto end(const HashMap<Key, T>& map)
{
   return map.CEnd();
}

} // namespace BIEngine
