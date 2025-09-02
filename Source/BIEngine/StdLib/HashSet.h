#pragma once

#include <functional>

#include "DynamicArray.h"
#include "ForwardList.h"

namespace BIEngine {

template <typename Key, typename Hasher = Hash<Key>, typename KeyEqual = std::equal_to<Key>>
class HashSet {
public:
   using ValueType = const Key;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstReference = const ValueType&;
   using ConstPointer = const ValueType*;
   using KeyType = Key;
   using HashType = Hasher;
   using KeyEqualType = KeyEqual;
   using SizeType = SizeT;

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

      ValueType& operator*() const
      {
         return *m_listItr;
      }

      ValueType* operator->() const
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
      ConstIterator(const HashSet<Key, Hasher, KeyEqual>* pHashSet, SizeT bucketIdx, typename ForwardList<Key>::ConstIterator listItr)
         : m_pHashSet(pHashSet), m_bucketIdx(bucketIdx), m_listItr(listItr)
      {
      }

   private:
      const HashSet<Key, Hasher, KeyEqual>* m_pHashSet;
      SizeT m_bucketIdx;
      typename ForwardList<Key>::ConstIterator m_listItr;
   };

   using Iterator = ConstIterator;

   HashSet();

   SizeType Size() const;
   bool Empty() const;

   Iterator Begin();
   Iterator End();

   ConstIterator CBegin() const;
   ConstIterator CEnd() const;

   Iterator Find(const KeyType& key);
   ConstIterator Find(const KeyType& key) const;

   void Clear();

   std::pair<Iterator, bool> Insert(const KeyType& key);
   std::pair<Iterator, bool> Insert(KeyType&& key);

   template <typename... U>
   std::pair<Iterator, bool> Emplace(U&&... args);

   SizeType Erase(const KeyType& key);
   Iterator Erase(Iterator itrToErase);

   float LoadFactor() const;
   float MaxLoadFactor() const;

private:
   SizeType getBucketIdxFromKey(const KeyType& key, SizeType n) const;

   void tryRehash();
   void rehash(SizeType n);

private:
   SizeType m_size = 0;
   DynamicArray<ForwardList<Key>> m_storage;
};

/*HashSet*/

template <typename Key, typename Hasher, typename KeyEqual>
HashSet<Key, Hasher, KeyEqual>::HashSet()
{
   m_storage.Resize(8);
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::SizeType HashSet<Key, Hasher, KeyEqual>::Size() const
{
   return m_size;
}

template <typename Key, typename Hasher, typename KeyEqual>
inline bool HashSet<Key, Hasher, KeyEqual>::Empty() const
{
   return m_size == 0;
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::Iterator HashSet<Key, Hasher, KeyEqual>::Begin()
{
   if (m_size == 0) {
      return End();
   }

   int bucketIdx = -1;
   while (m_storage[++bucketIdx].Empty()) {
   }

   return Iterator(this, bucketIdx, m_storage[bucketIdx].CBegin());
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::Iterator HashSet<Key, Hasher, KeyEqual>::End()
{
   return Iterator(this, m_storage.Size(), m_storage[0].CEnd());
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::ConstIterator HashSet<Key, Hasher, KeyEqual>::CBegin() const
{
   if (m_size == 0) {
      return CEnd();
   }

   int bucketIdx = -1;
   while (m_storage[++bucketIdx].Empty()) {
   }

   return ConstIterator(this, bucketIdx, m_storage[bucketIdx].CBegin());
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::ConstIterator HashSet<Key, Hasher, KeyEqual>::CEnd() const
{
   return ConstIterator(this, m_storage.Size(), m_storage[0].CEnd());
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::Iterator HashSet<Key, Hasher, KeyEqual>::Find(const KeyType& key)
{
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
      if (KeyEqual()(*itr, key)) {
         return Iterator(this, bucketIdx, itr);
      }
   }

   return End();
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::ConstIterator HashSet<Key, Hasher, KeyEqual>::Find(const KeyType& key) const
{
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
      if (KeyEqual()(*itr, key)) {
         return ConstIterator(this, bucketIdx, itr);
      }
   }

   return CEnd();
}

template <typename Key, typename Hasher, typename KeyEqual>
inline void HashSet<Key, Hasher, KeyEqual>::Clear()
{
   for (auto itr = m_storage.Begin(); itr != m_storage.End(); ++itr) {
      itr->Clear();
   }

   m_size = 0;
}

template <typename Key, typename Hasher, typename KeyEqual>
inline std::pair<typename HashSet<Key, Hasher, KeyEqual>::Iterator, bool> HashSet<Key, Hasher, KeyEqual>::Insert(const KeyType& key)
{
   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].PushFront(key);
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename Hasher, typename KeyEqual>
inline std::pair<typename HashSet<Key, Hasher, KeyEqual>::Iterator, bool> HashSet<Key, Hasher, KeyEqual>::Insert(KeyType&& key)
{
   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].PushFront(std::forward<KeyType>(key));
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename Hasher, typename KeyEqual>
template <typename... U>
inline std::pair<typename HashSet<Key, Hasher, KeyEqual>::Iterator, bool> HashSet<Key, Hasher, KeyEqual>::Emplace(U&&... args)
{
   ForwardList<Key> temp;
   temp.EmplaceFront(std::forward<U>(args)...);
   const Key& key = *temp.Begin();

   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].SpliceAfter(m_storage[bucketIdx].BeforeBegin(), temp, temp.BeforeBegin());
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::SizeType HashSet<Key, Hasher, KeyEqual>::Erase(const KeyType& key)
{
   SizeType retVal = 0;
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

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

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::Iterator HashSet<Key, Hasher, KeyEqual>::Erase(Iterator itrToErase)
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

template <typename Key, typename Hasher, typename KeyEqual>
inline float HashSet<Key, Hasher, KeyEqual>::LoadFactor() const
{
   return static_cast<float>(Size()) / m_storage.Size();
}

template <typename Key, typename Hasher, typename KeyEqual>
inline float HashSet<Key, Hasher, KeyEqual>::MaxLoadFactor() const
{
   return 0.8f;
}

template <typename Key, typename Hasher, typename KeyEqual>
inline typename HashSet<Key, Hasher, KeyEqual>::SizeType HashSet<Key, Hasher, KeyEqual>::getBucketIdxFromKey(const KeyType& key, SizeType n) const
{
   const SizeType hash = Hasher()(key);
   return hash & (n - 1);
}

template <typename Key, typename Hasher, typename KeyEqual>
inline void HashSet<Key, Hasher, KeyEqual>::tryRehash()
{
   if (MaxLoadFactor() > LoadFactor()) {
      return;
   }

   rehash(m_storage.Size() * 2);
}

template <typename Key, typename Hasher, typename KeyEqual>
inline void HashSet<Key, Hasher, KeyEqual>::rehash(SizeType n)
{
   DynamicArray<ForwardList<Key>> newStorage(n);

   for (int bucketIdx = 0; bucketIdx < m_storage.Size(); ++bucketIdx) {
      for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
         SizeType newIdx = getBucketIdxFromKey(*itr, newStorage.Size());

         ++itr;
         newStorage[newIdx].SpliceAfter(newStorage[newIdx].BeforeBegin(), m_storage[bucketIdx], m_storage[bucketIdx].BeforeBegin());
      }
   }

   m_storage = std::move(newStorage);
}

/*foreach loop helpers*/

template <typename Key, typename Hasher, typename KeyEqual>
auto begin(HashSet<Key, Hasher, KeyEqual>& set)
{
   return set.Begin();
}

template <typename Key, typename Hasher, typename KeyEqual>
auto end(HashSet<Key, Hasher, KeyEqual>& set)
{
   return set.End();
}

template <typename Key, typename Hasher, typename KeyEqual>
auto begin(const HashSet<Key, Hasher, KeyEqual>& set)
{
   return set.CBegin();
}

template <typename Key, typename Hasher, typename KeyEqual>
auto end(const HashSet<Key, Hasher, KeyEqual>& set)
{
   return set.CEnd();
}

} // namespace BIEngine
