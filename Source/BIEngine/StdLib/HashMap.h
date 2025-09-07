#pragma once

#include "Functional.h"
#include "DynamicArray.h"
#include "ForwardList.h"

namespace BIEngine {

template <typename Key, typename T, typename Hasher = Hash<Key>, typename KeyEqual = EqualTo<Key>>
class HashMap {

   class ConstHashMapIterator : public IteratorBase<ForwardIteratorTag, std::pair<const Key, T>> {
      friend class HashMap;

   public:
      ConstHashMapIterator& operator=(const ConstHashMapIterator& rhs)
      {
         m_pHashMap = rhs.m_pHashMap;
         m_bucketIdx = rhs.m_bucketIdx;
         m_listItr = rhs.m_listItr;

         return *this;
      }

      ConstHashMapIterator& operator++()
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

      ConstHashMapIterator operator++(int)
      {
         ConstHashMapIterator old = *this;
         operator++();
         return old;
      }

      const typename IteratorTraits<ConstHashMapIterator>::Reference operator*() const
      {
         return *m_listItr;
      }

      const typename IteratorTraits<ConstHashMapIterator>::Pointer operator->() const
      {
         return m_listItr.operator->();
      }

      friend bool operator==(const ConstHashMapIterator& lhs, const ConstHashMapIterator& rhs)
      {
         return lhs.m_bucketIdx == rhs.m_bucketIdx && lhs.m_listItr == rhs.m_listItr;
      }

      friend bool operator!=(const ConstHashMapIterator& lhs, const ConstHashMapIterator& rhs)
      {
         return !(lhs == rhs);
      }

   protected:
      ConstHashMapIterator(const HashMap<Key, T>* pHashMap, SizeT bucketIdx, typename ForwardList<std::pair<const Key, T>>::ConstIterator listItr)
         : m_pHashMap(pHashMap), m_bucketIdx(bucketIdx), m_listItr(listItr)
      {
      }

   protected:
      const HashMap<Key, T>* m_pHashMap;
      SizeT m_bucketIdx;
      typename ForwardList<std::pair<const Key, T>>::ConstIterator m_listItr;
   };

   class HashMapIterator : public ConstHashMapIterator {
      friend class HashMap;

   public:
      typename IteratorTraits<HashMapIterator>::Reference operator*()
      {
         return *this->m_listItr;
      }

      typename IteratorTraits<HashMapIterator>::Pointer operator->()
      {
         return this->m_listItr.operator->();
      }

   protected:
      HashMapIterator(HashMap<Key, T>* pHashMap, SizeT bucketIdx, typename ForwardList<std::pair<const Key, T>>::Iterator listItr)
         : ConstHashMapIterator(pHashMap, bucketIdx, listItr)
      {
      }
   };

public:
   using ValueType = std::pair<const Key, T>;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstReference = const ValueType&;
   using ConstPointer = const ValueType*;
   using KeyType = Key;
   using MappedType = T;
   using HashType = Hasher;
   using KeyEqualType = KeyEqual;
   using SizeType = SizeT;
   using Iterator = HashMapIterator;
   using ConstIterator = ConstHashMapIterator;

   HashMap();

   SizeType Size() const;
   bool Empty() const;

   Iterator Begin();
   Iterator End();

   ConstIterator Begin() const;
   ConstIterator End() const;

   ConstIterator CBegin() const;
   ConstIterator CEnd() const;

   MappedType& operator[](const KeyType& key);

   Iterator Find(const KeyType& key);
   ConstIterator Find(const KeyType& key) const;

   void Clear();

   std::pair<Iterator, bool> Insert(const KeyType& key, const MappedType& val);
   std::pair<Iterator, bool> Insert(const KeyType& key, MappedType&& val);

   template <typename... U>
   std::pair<Iterator, bool> Emplace(const KeyType& key, U&&... args);

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
   DynamicArray<ForwardList<ValueType>> m_storage;
};

/*HashMap*/

template <typename Key, typename T, typename Hasher, typename KeyEqual>
HashMap<Key, T, Hasher, KeyEqual>::HashMap()
{
   m_storage.Resize(8);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::SizeType HashMap<Key, T, Hasher, KeyEqual>::Size() const
{
   return m_size;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline bool HashMap<Key, T, Hasher, KeyEqual>::Empty() const
{
   return m_size == 0;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::Iterator HashMap<Key, T, Hasher, KeyEqual>::Begin()
{
   if (m_size == 0) {
      return End();
   }

   int bucketIdx = -1;
   while (m_storage[++bucketIdx].Empty()) {
   }

   return Iterator(this, bucketIdx, m_storage[bucketIdx].Begin());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::Iterator HashMap<Key, T, Hasher, KeyEqual>::End()
{
   return Iterator(this, m_storage.Size(), m_storage[0].End());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::ConstIterator HashMap<Key, T, Hasher, KeyEqual>::Begin() const
{
   if (m_size == 0) {
      return End();
   }

   int bucketIdx = -1;
   while (m_storage[++bucketIdx].Empty()) {
   }

   return ConstIterator(this, bucketIdx, m_storage[bucketIdx].Begin());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::ConstIterator HashMap<Key, T, Hasher, KeyEqual>::End() const
{
   return ConstIterator(this, m_storage.Size(), m_storage[0].End());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::ConstIterator HashMap<Key, T, Hasher, KeyEqual>::CBegin() const
{
   if (m_size == 0) {
      return CEnd();
   }

   int bucketIdx = -1;
   while (m_storage[++bucketIdx].Empty()) {
   }

   return ConstIterator(this, bucketIdx, m_storage[bucketIdx].CBegin());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::ConstIterator HashMap<Key, T, Hasher, KeyEqual>::CEnd() const
{
   return ConstIterator(this, m_storage.Size(), m_storage[0].CEnd());
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
typename HashMap<Key, T, Hasher, KeyEqual>::MappedType& HashMap<Key, T, Hasher, KeyEqual>::operator[](const KeyType& key)
{
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End(); ++itr) {
      if (KeyEqual()(itr->first, key)) {
         return itr->second;
      }
   }

   return Emplace(key, MappedType{}).first->second;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::Iterator HashMap<Key, T, Hasher, KeyEqual>::Find(const KeyType& key)
{
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End(); ++itr) {
      if (KeyEqual()(itr->first, key)) {
         return Iterator(this, bucketIdx, itr);
      }
   }

   return End();
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::ConstIterator HashMap<Key, T, Hasher, KeyEqual>::Find(const KeyType& key) const
{
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   for (auto itr = m_storage[bucketIdx].CBegin(); itr != m_storage[bucketIdx].CEnd(); ++itr) {
      if (KeyEqual()(itr->first, key)) {
         return ConstIterator(this, bucketIdx, itr);
      }
   }

   return CEnd();
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline void HashMap<Key, T, Hasher, KeyEqual>::Clear()
{
   for (auto itr = m_storage.Begin(); itr != m_storage.End(); ++itr) {
      itr->Clear();
   }

   m_size = 0;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline std::pair<typename HashMap<Key, T, Hasher, KeyEqual>::Iterator, bool> HashMap<Key, T, Hasher, KeyEqual>::Insert(const KeyType& key, const MappedType& val)
{
   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].PushFront({key, val});
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline std::pair<typename HashMap<Key, T, Hasher, KeyEqual>::Iterator, bool> HashMap<Key, T, Hasher, KeyEqual>::Insert(const KeyType& key, MappedType&& val)
{
   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].PushFront({key, std::forward<T>(val)});
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
template <typename... U>
inline std::pair<typename HashMap<Key, T, Hasher, KeyEqual>::Iterator, bool> HashMap<Key, T, Hasher, KeyEqual>::Emplace(const KeyType& key, U&&... args)
{
   Iterator itr = Find(key);
   if (itr != End()) {
      return std::pair<Iterator, bool>(itr, false);
   }

   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   m_storage[bucketIdx].EmplaceFront(key, std::forward<U>(args)...);
   ++m_size;

   tryRehash();

   return std::pair<Iterator, bool>(Find(key), true);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::SizeType HashMap<Key, T, Hasher, KeyEqual>::Erase(const KeyType& key)
{
   SizeType retVal = 0;
   const SizeType bucketIdx = getBucketIdxFromKey(key, m_storage.Size());

   auto itrBefore = m_storage[bucketIdx].BeforeBegin();

   for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
      if (KeyEqual()(itr->first, key)) {
         itr = m_storage[bucketIdx].EraseAfter(itrBefore);
         ++retVal;
      } else {
         ++itr;
         ++itrBefore;
      }
   }

   return retVal;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::Iterator HashMap<Key, T, Hasher, KeyEqual>::Erase(Iterator itrToErase)
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

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline float HashMap<Key, T, Hasher, KeyEqual>::LoadFactor() const
{
   return static_cast<float>(Size()) / m_storage.Size();
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline float HashMap<Key, T, Hasher, KeyEqual>::MaxLoadFactor() const
{
   return 0.8f;
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline typename HashMap<Key, T, Hasher, KeyEqual>::SizeType HashMap<Key, T, Hasher, KeyEqual>::getBucketIdxFromKey(const KeyType& key, SizeType n) const
{
   const SizeType hash = Hasher()(key);
   return hash & (n - 1);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline void HashMap<Key, T, Hasher, KeyEqual>::tryRehash()
{
   if (MaxLoadFactor() > LoadFactor()) {
      return;
   }

   rehash(m_storage.Size() * 2);
}

template <typename Key, typename T, typename Hasher, typename KeyEqual>
inline void HashMap<Key, T, Hasher, KeyEqual>::rehash(SizeType n)
{
   DynamicArray<ForwardList<ValueType>> newStorage(n);

   for (int bucketIdx = 0; bucketIdx < m_storage.Size(); ++bucketIdx) {
      for (auto itr = m_storage[bucketIdx].Begin(); itr != m_storage[bucketIdx].End();) {
         SizeType newIdx = getBucketIdxFromKey(itr->first, newStorage.Size());

         ++itr;
         newStorage[newIdx].SpliceAfter(newStorage[newIdx].BeforeBegin(), m_storage[bucketIdx], m_storage[bucketIdx].BeforeBegin());
      }
   }

   m_storage = std::move(newStorage);
}

/*foreach loop helpers*/

template <typename Key, typename T, typename Hasher = Hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto begin(HashMap<Key, T, Hasher, KeyEqual>& map)
{
   return map.Begin();
}

template <typename Key, typename T, typename Hasher = Hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto end(HashMap<Key, T, Hasher, KeyEqual>& map)
{
   return map.End();
}

template <typename Key, typename T, typename Hasher = Hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto begin(const HashMap<Key, T, Hasher, KeyEqual>& map)
{
   return map.CBegin();
}

template <typename Key, typename T, typename Hasher = Hash<Key>, typename KeyEqual = std::equal_to<Key>>
auto end(const HashMap<Key, T, Hasher, KeyEqual>& map)
{
   return map.CEnd();
}

} // namespace BIEngine
