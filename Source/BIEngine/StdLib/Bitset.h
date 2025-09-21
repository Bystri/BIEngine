#pragma once

#include <memory>

#include "Assert.h"
#include "../StdLib/String.h"
#include "../StdLib/StdLib.h"

namespace BIEngine {

template <SizeT N>
class Bitset {
public:
   class Reference {
      friend class Bitset;

   public:
      Reference(const Reference& other) = default;

      Reference& operator=(bool val)
      {
         m_pBitset->Set(m_idx, val);

         return *this;
      }

      Reference& operator=(const Reference& val)
      {
         m_pBitset->Set(m_idx, val);

         return *this;
      }

      operator bool() const
      {
         return m_pBitset->Test(m_idx);
      }

      bool operator~() const
      {
         return !m_pBitset->Test(m_idx);
      }

      Reference& Flip()
      {
         m_pBitset->Flip(m_idx);
         return *this;
      }

   private:
      Reference(Bitset* pBitset, SizeT idx)
         : m_pBitset(pBitset), m_idx(idx)
      {
      }

   private:
      Bitset* const m_pBitset;
      const SizeT m_idx;
   };

private:
   using DataStorageType = unsigned char;

public:
   Bitset();
   Bitset(unsigned long long val);

   template <typename CharT>
   explicit Bitset(const CharT* str, SizeT n = -1, CharT zero = CharT('0'), CharT one = CharT('1'));

   template <typename CharT>
   explicit Bitset(
      const BasicString<CharT>& str,
      typename BasicString<CharT>::SizeType pos = 0,
      typename BasicString<CharT>::SizeType n = BasicString<CharT>::NPos,
      CharT zero = CharT('0'), CharT one = CharT('1'));

   bool Test(SizeT idx) const;

   bool operator[](SizeT idx) const;
   Reference operator[](SizeT idx);

   bool operator==(const Bitset& rhs) const;
   bool operator!=(const Bitset& rhs) const;

   bool All() const;
   bool Any() const;
   bool None();

   SizeT Count() const;
   SizeT Size() const;

   Bitset& operator&=(const Bitset& other);
   Bitset& operator|=(const Bitset& other);
   Bitset& operator^=(const Bitset& other);
   Bitset operator~() const;

   Bitset& Set();
   Bitset& Set(SizeT idx, bool val = true);

   Bitset& Reset();
   Bitset& Reset(SizeT idx);

   Bitset& Flip();
   Bitset& Flip(SizeT idx);

private:
   template <typename CharT>
   void fillDataFromRawStr(const CharT* str, SizeT strLen, CharT zero, CharT one);

   std::pair<SizeT, SizeT> getChunkAndBitIdx(SizeT idx) const;

private:
   static constexpr SizeT BITS_IN_ONE_CHUNK_COUNT = sizeof(DataStorageType) * 8;
   static constexpr SizeT CHUNK_NUM = ((N - 1) / BITS_IN_ONE_CHUNK_COUNT) + 1;

   static constexpr DataStorageType MASK_ALL_BITS_CHUNK = ~DataStorageType();
   static constexpr DataStorageType MASK_ALL_BITS_LAST_CHUNK = ~(MASK_ALL_BITS_CHUNK << (N - (N / BITS_IN_ONE_CHUNK_COUNT * BITS_IN_ONE_CHUNK_COUNT)));

   DataStorageType m_chunkMap[CHUNK_NUM];
};

/*Bitset*/

template <SizeT N>
Bitset<N>::Bitset()
{
   std::memset(m_chunkMap, 0, sizeof(DataStorageType) * CHUNK_NUM);
}

template <SizeT N>
Bitset<N>::Bitset(unsigned long long val)
   : Bitset()
{
   SizeT chunkIdx = 0;
   SizeT bitIdx = 0;
   while (val) {
      if (bitIdx >= BITS_IN_ONE_CHUNK_COUNT) {
         ++chunkIdx;
         bitIdx = 0;
      }

      if (val & 1) {
         m_chunkMap[chunkIdx] |= (1 << bitIdx);
      }

      val >>= 1;
      ++bitIdx;
   }
}

template <SizeT N>
template <typename CharT>
Bitset<N>::Bitset(const CharT* str, SizeT n /*= -1*/, CharT zero /*= CharT('0')*/, CharT one /*= CharT('1')*/)
   : Bitset()
{
   SizeT strLen = n;
   if (strLen == -1) {
      strLen = 0;
      const CharT* strPtr = str;
      while (*strPtr != CharT()) {
         ++strLen;
         ++strPtr;
      }
   }

   fillDataFromRawStr(str, strLen, zero, one);
}

template <SizeT N>
template <typename CharT>
Bitset<N>::Bitset(
   const BasicString<CharT>& str,
   typename BasicString<CharT>::SizeType pos /*= 0*/,
   typename BasicString<CharT>::SizeType n /*= BasicString<CharT>::NPos*/,
   CharT zero /*= CharT('0')*/, CharT one /*= CharT('1')*/)
   : Bitset()
{
   SizeT strLen = n;
   if (strLen == BasicString<CharT>::NPos) {
      strLen = str.Size();
   }

   fillDataFromRawStr(str.CStr() + pos, strLen - pos, zero, one);
}

template <SizeT N>
inline bool Bitset<N>::Test(SizeT idx) const
{
   const auto chunkAndBitIdx = getChunkAndBitIdx(idx);

   return m_chunkMap[chunkAndBitIdx.first] & (1 << chunkAndBitIdx.second);
}

template <SizeT N>
inline bool Bitset<N>::operator[](SizeT idx) const
{
   return Test(idx);
}

template <SizeT N>
inline typename Bitset<N>::Reference Bitset<N>::operator[](SizeT idx)
{
   return Reference(this, idx);
}

template <SizeT N>
inline bool Bitset<N>::operator==(const Bitset& rhs) const
{
   for (int i = 0; i < CHUNK_NUM; ++i) {
      if (m_chunkMap[i] != rhs.m_chunkMap[i]) {
         return false;
      }
   }

   return true;
}

template <SizeT N>
inline bool Bitset<N>::operator!=(const Bitset& rhs) const
{
   return !operator==(rhs);
}

template <SizeT N>
inline bool Bitset<N>::All() const
{
   for (int i = 0; i < CHUNK_NUM - 1; ++i) {
      if (m_chunkMap[i] != MASK_ALL_BITS_CHUNK) {
         return false;
      }
   }

   return m_chunkMap[CHUNK_NUM - 1] == MASK_ALL_BITS_LAST_CHUNK;
}

template <SizeT N>
inline bool Bitset<N>::Any() const
{
   for (int i = 0; i < CHUNK_NUM; ++i) {
      if (m_chunkMap[i] != DataStorageType()) {
         return true;
      }
   }

   return false;
}

template <SizeT N>
inline bool Bitset<N>::None()
{
   return !Any();
}

template <SizeT N>
inline SizeT Bitset<N>::Count() const
{
   SizeT counter = 0;
   for (int i = 0; i < CHUNK_NUM; ++i) {
      for (int j = 0; j < BITS_IN_ONE_CHUNK_COUNT; ++j) {
         if (m_chunkMap[i] & (1 << j)) {
            ++counter;
         }
      }
   }

   return counter;
}

template <SizeT N>
inline SizeT Bitset<N>::Size() const
{
   return N;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::operator&=(const Bitset& other)
{
   for (int i = 0; i < CHUNK_NUM; ++i) {
      m_chunkMap[i] &= other.m_chunkMap[i];
   }

   m_chunkMap[CHUNK_NUM - 1] &= MASK_ALL_BITS_LAST_CHUNK;

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::operator|=(const Bitset& other)
{
   for (int i = 0; i < CHUNK_NUM; ++i) {
      m_chunkMap[i] |= other.m_chunkMap[i];
   }

   m_chunkMap[CHUNK_NUM - 1] &= MASK_ALL_BITS_LAST_CHUNK;

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::operator^=(const Bitset& other)
{
   for (int i = 0; i < CHUNK_NUM; ++i) {
      m_chunkMap[i] ^= other.m_chunkMap[i];
   }

   m_chunkMap[CHUNK_NUM - 1] &= MASK_ALL_BITS_LAST_CHUNK;

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset Bitset<N>::operator~() const
{
   Bitset thisCopy;

   for (int i = 0; i < CHUNK_NUM; ++i) {
      thisCopy.m_chunkMap[i] = ~m_chunkMap[i];
   }

   thisCopy.m_chunkMap[CHUNK_NUM - 1] &= MASK_ALL_BITS_LAST_CHUNK;

   return thisCopy;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Set()
{
   for (int i = 0; i < CHUNK_NUM - 1; ++i) {
      m_chunkMap[i] |= MASK_ALL_BITS_CHUNK;
   }

   m_chunkMap[CHUNK_NUM - 1] |= MASK_ALL_BITS_LAST_CHUNK;

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Set(SizeT idx, bool val /*= true*/)
{
   const auto chunkAndBitIdx = getChunkAndBitIdx(idx);

   if (val) {
      m_chunkMap[chunkAndBitIdx.first] |= (1 << chunkAndBitIdx.second);
   } else {
      m_chunkMap[chunkAndBitIdx.first] &= (~(1 << chunkAndBitIdx.second));
   }

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Reset()
{
   std::memset(m_chunkMap, 0, sizeof(DataStorageType) * CHUNK_NUM);

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Reset(SizeT idx)
{
   const auto chunkAndBitIdx = getChunkAndBitIdx(idx);

   m_chunkMap[chunkAndBitIdx.first] &= (~(1 << chunkAndBitIdx.second));

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Flip()
{
   for (int i = 0; i < CHUNK_NUM - 1; ++i) {
      m_chunkMap[i] = ~m_chunkMap[i];
   }

   m_chunkMap[CHUNK_NUM - 1] = ~m_chunkMap[CHUNK_NUM - 1];
   m_chunkMap[CHUNK_NUM - 1] &= MASK_ALL_BITS_LAST_CHUNK;

   return *this;
}

template <SizeT N>
inline typename Bitset<N>::Bitset& Bitset<N>::Flip(SizeT idx)
{
   const auto chunkAndBitIdx = getChunkAndBitIdx(idx);

   if (m_chunkMap[chunkAndBitIdx.first] & (1 << chunkAndBitIdx.second)) {
      m_chunkMap[chunkAndBitIdx.first] &= (~(1 << chunkAndBitIdx.second));
   } else {
      m_chunkMap[chunkAndBitIdx.first] |= (1 << chunkAndBitIdx.second);
   }

   return *this;
}

template <SizeT N>
template <typename CharT>
inline void Bitset<N>::fillDataFromRawStr(const CharT* str, SizeT strLen, CharT zero, CharT one)
{
   SizeT chunkIdx = 0;
   SizeT bitIdx = 0;
   for (int i = strLen - 1; i >= 0; --i, ++bitIdx) {
      if (bitIdx >= BITS_IN_ONE_CHUNK_COUNT) {
         ++chunkIdx;
         bitIdx = 0;
      }

      if (str[i] == zero) {
         continue;
      }

      if (str[i] == one) {
         m_chunkMap[chunkIdx] |= (1 << bitIdx);
         continue;
      }

      Assert(false, "Invalid argument; Str for Bitset contains character than nor one - %c, not zero - %c; Str is %s", one, zero, str);
   }
}

template <SizeT N>
inline std::pair<SizeT, SizeT> Bitset<N>::getChunkAndBitIdx(SizeT idx) const
{
   const SizeT chunkIdx = idx / BITS_IN_ONE_CHUNK_COUNT;
   const SizeT bitIdx = idx - chunkIdx * BITS_IN_ONE_CHUNK_COUNT;
   return std::make_pair(chunkIdx, bitIdx);
}

/*Global operators*/

template <SizeT N>
Bitset<N> operator&(const Bitset<N>& lhs, const Bitset<N>& rhs)
{
   Bitset<N> ret(lhs);
   ret &= rhs;

   return ret;
}

template <SizeT N>
Bitset<N> operator|(const Bitset<N>& lhs, const Bitset<N>& rhs)
{
   Bitset<N> ret(lhs);
   ret |= rhs;

   return ret;
}

template <SizeT N>
Bitset<N> operator^(const Bitset<N>& lhs, const Bitset<N>& rhs)
{
   Bitset<N> ret(lhs);
   ret ^= rhs;

   return ret;
}

} // namespace BIEngine
