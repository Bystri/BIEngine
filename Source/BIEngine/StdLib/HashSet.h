#pragma once

#include <functional>

#include "DynamicArray.h"
#include "ForwardList.h"

namespace BIEngine {

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
class HashSet {
public:
   class ConstIterator {
      friend class HashSet;

   public:
      ConstIterator& operator=(const ConstIterator& rhs)
      {
         m_pHashSet = rhs.m_pHashSet;
         m_bucketIdx = rhs.m_bucketIdx;
         m_listItr = rhs.m_listItr;

         return *this;
      }

      ConstIterator& operator++()
      {
         ++m_listItr;
         if (m_pHashSet->m_storage[m_bucketIdx].CEnd() == m_listItr) {
            while (++m_bucketIdx < m_pHashSet->m_storage.Size() && m_pHashSet->m_storage[m_bucketIdx].Empty()) {
            }

            if (m_bucketIdx >= m_pHashSet->m_storage.Size()) {
               m_listItr = m_pHashSet->m_storage[0].CEnd();
            } else {
               m_listItr = m_pHashSet->m_storage[m_bucketIdx].CBegin();
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

      const Key& operator*() const
      {
         return *m_listItr;
      }

      const Key* operator->() const
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
      ConstIterator(const HashSet<Key, Hash, KeyEqual>* pHashSet, SizeT bucketIdx, typename ForwardList<Key>::ConstIterator listItr)
         : m_pHashSet(pHashSet), m_bucketIdx(bucketIdx), m_listItr(listItr)
      {
      }

   private:
      const HashSet<Key, Hash, KeyEqual>* m_pHashSet;
      SizeT m_bucketIdx;
      typename ForwardList<Key>::ConstIterator m_listItr;
   };

   using Iterator = ConstIterator;

   HashSet()
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

      return Iterator(this, bucketIdx, m_storage[bucketIdx].CBegin());
   }

   Iterator End()
   {
      return Iterator(this, m_storage.Size(), m_storage[0].CEnd());
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

   Iterator Find(const Key& key)
   {
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
         if (KeyEqual()(*itr, key)) {
            return Iterator(this, bucketIdx, itr);
         }
      }

      return End();
   }

   ConstIterator Find(const Key& key) const
   {
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
         if (KeyEqual()(*itr, key)) {
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

   std::pair<Iterator, bool> Insert(const Key& key)
   {
      Iterator itr = Find(key);
      if (itr != End()) {
         return std::pair<Iterator, bool>(itr, false);
      }

      tryRehash();

      const SizeT bucketIdx = getBucketIdxFromKey(key);

      m_storage[bucketIdx].PushFront(key);
      ++m_size;

      return std::pair<Iterator, bool>(Iterator(this, bucketIdx, m_storage[bucketIdx].CBegin()), true);
   }

   template <typename... U>
   std::pair<Iterator, bool> Emplace(U&&... args)
   {
      ForwardList<Key> temp;
      temp.EmplaceFront(std::forward<U>(args)...);
      const Key& key = *temp.Begin();

      Iterator itr = Find(key);
      if (itr != End()) {
         return std::pair<Iterator, bool>(itr, false);
      }

      tryRehash();

      const SizeT bucketIdx = getBucketIdxFromKey(key);

      m_storage[bucketIdx].SpliceAfter(m_storage[bucketIdx].BeforeBegin(), temp, temp.BeforeBegin());
      ++m_size;

      return std::pair<Iterator, bool>(Iterator(this, bucketIdx, m_storage[bucketIdx].CBegin()), true);
   }

   SizeT Erase(const Key& key)
   {
      SizeT retVal = 0;
      const SizeT bucketIdx = getBucketIdxFromKey(key);

      auto itrBefore = m_storage[bucketIdx].CBeforeBegin();

      for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd();) {
         if (KeyEqual()(*itr, key)) {
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
      auto itrBefore = m_storage[itrToErase.m_bucketIdx].CBeforeBegin();
      auto itr = m_storage[itrToErase.m_bucketIdx].CBegin();
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
      const SizeT hash = Hash()(key);
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
      DynamicArray<ForwardList<Key>> newStorage(n);

      for (int bucketIdx = 0; bucketIdx < m_storage.Size(); ++bucketIdx) {
         for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
            const SizeT hash = Hash()(*itr);
            SizeT newIdx = hash % newStorage.Size();

            ++itr;
            newStorage[newIdx].SpliceAfter(newStorage[newIdx].BeforeBegin(), m_storage[bucketIdx], m_storage[bucketIdx].BeforeBegin());
         }
      }

      m_storage = std::move(newStorage);
   }

private:
   SizeT m_size = 0;
   DynamicArray<ForwardList<Key>> m_storage;
};

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto begin(HashSet<Key, Hash, KeyEqual>& set)
{
   return set.Begin();
}

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto end(HashSet<Key, Hash, KeyEqual>& set)
{
   return set.End();
}

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto begin(const HashSet<Key, Hash, KeyEqual>& set)
{
   return set.CBegin();
}

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto end(const HashSet<Key, Hash, KeyEqual>& set)
{
   return set.CEnd();
}

} // namespace BIEngine
