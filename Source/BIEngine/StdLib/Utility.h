#pragma once

#include <memory>

#include "StdLib.h"

namespace BIEngine {

template <typename T>
struct Hash;

// Stolen from Boost
template <typename T>
void HashCombine(SizeT& hash, const T& val)
{
   hash ^= Hash<T>()(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
}

// Stolen from Boost
template <typename Iterator>
SizeT HashRange(Iterator begin, Iterator end)
{
   SizeT hash = 0;

   Iterator itr = begin;
   while (itr != end) {
      HashCombine(hash, *itr);
      ++itr;
   }

   return hash;
}

template <>
struct Hash<bool> {
   SizeT operator()(bool val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<char> {
   SizeT operator()(char val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<signed char> {
   SizeT operator()(signed char val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<unsigned char> {
   SizeT operator()(unsigned char val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<signed short> {
   SizeT operator()(signed short val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<unsigned short> {
   SizeT operator()(unsigned short val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<signed int> {
   SizeT operator()(signed int val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<unsigned int> {
   SizeT operator()(unsigned int val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<signed long> {
   SizeT operator()(signed long val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<unsigned long> {
   SizeT operator()(unsigned long val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<signed long long> {
   SizeT operator()(signed long long val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <>
struct Hash<unsigned long long> {
   SizeT operator()(unsigned long long val) const
   {
      return static_cast<SizeT>(val);
   }
};

template <typename T>
struct Hash<T*> {
   SizeT operator()(T* val) const
   {
      return SizeT(uintptr_t(val) / sizeof(T*));
   }
};

template <typename T, SizeT N>
struct Hash<T[N]> {
   SizeT operator()(T val[]) const
   {
      SizeT hash = 0;
      for (int i = 0; i < N; ++i) {
         HashCombine(hash, val[i]);
      }
      return hash;
   }
};

} // namespace BIEngine
