#pragma once

#include <memory>

#include "StdLib.h"

namespace BIEngine {

/*Pair*/

template <typename T1, typename T2>
struct Pair {
   using FirstType = T1;
   using SecondType = T2;

   Pair() = default;

   Pair(const T1& x, const T2& y)
      : first(x), second(y)
   {
   }

   template <class U1, class U2>
   Pair(U1&& x, U2&& y)
      : first(std::move(x)), second(std::move(y))
   {
   }

   template <class U1, class U2>
   Pair(const Pair<U1, U2>& p)
      : first(p.first), second(p.second)
   {
   }

   template <class U1, class U2>
   Pair(Pair<U1, U2>&& p)
      : first(std::move(p.first)), second(std::move(p.second))
   {
   }

   Pair(const Pair& p) = default;
   Pair(Pair&& p) = default;

   Pair& operator=(const Pair& other)
   {
      first = other.first;
      second = other.second;

      return *this;
   }

   template <class U1, class U2>
   Pair& operator=(const Pair<U1, U2>& other)
   {
      first = other.first;
      second = other.second;

      return *this;
   }

   Pair& operator=(Pair&& other)
   {
      if (this == &other) {
         return *this;
      }

      first = std::move(other.first);
      second = std::move(other.second);

      return *this;
   }

   template <class U1, class U2>
   Pair& operator=(Pair<U1, U2>&& other)
   {
      first = std::move(other.first);
      second = std::move(other.second);

      return *this;
   }

   T1 first{};
   T2 second{};
};

template <class T1, class T2, class U1, class U2>
bool operator==(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1, class T2, class U1, class U2>
bool operator!=(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   return !(lhs == rhs);
}

template <class T1, class T2, class U1, class U2>
bool operator<(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   if (lhs.first == rhs.first) {
      return lhs.second < rhs.second;
   }

   return lhs.first < rhs.first;
}

template <class T1, class T2, class U1, class U2>
bool operator<=(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   return !(lhs > rhs);
}

template <class T1, class T2, class U1, class U2>
bool operator>(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   if (lhs.first == rhs.first) {
      return lhs.second > rhs.second;
   }

   return lhs.first > rhs.first;
}

template <class T1, class T2, class U1, class U2>
bool operator>=(const Pair<T1, T2>& lhs, const Pair<U1, U2>& rhs)
{
   return !(lhs < rhs);
}

template <typename T1, typename T2>
Pair<T1, T2> MakePair(const T1& x, const T2& y)
{
   return Pair<T1, T2>(x, y);
}

template <typename T1, typename T2>
Pair<T1, T2> MakePair(T1&& x, T2&& y)
{
   return Pair<T1, T2>(std::forward<T1>(x), std::forward<T2>(y));
}

/*Hash*/

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
