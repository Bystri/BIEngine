#pragma once

#include <cstring>
#include <memory>
#include <initializer_list>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

#include "StdLib.h"
#include "Utility.h"

namespace BIEngine {

template <typename CharT>
class BasicString {
public:
   using ValueType = CharT;
   using Iterator = CharT*;
   using ConstIterator = const CharT*;

   static const SizeT NPos = static_cast<SizeT>(-1);

   // Stole the idea from EASTL string.h
   struct CtorSprintf {};

   BasicString() = default;

   BasicString(const CharT* str)
   {
      SizeT strSize = 0;
      while (str[strSize++] != CharT()) {
      }
      --strSize;

      Reserve(strSize);
      for (int i = 0; i < strSize; ++i) {
         PushBack(str[i]);
      }
   }

   BasicString(const CharT* str, SizeT n)
   {
      Reserve(n);
      for (int i = 0; i < n; ++i) {
         PushBack(str[i]);
      }
   }

   explicit BasicString(SizeT n)
   {
      Reserve(n);
      for (int i = 0; i < n; ++i) {
         PushBack(CharT());
      }
   }

   BasicString(SizeT n, const CharT& val)
   {
      Reserve(n);
      for (int i = 0; i < n; ++i) {
         PushBack(val);
      }
   }

   BasicString(std::initializer_list<CharT> l)
   {
      Reserve(l.size());
      for (const auto& val : l) {
         PushBack(val);
      }
   }

   BasicString(CtorSprintf, const CharT* pFormat, ...)
   {
      va_list args;
      va_start(args, pFormat);
      AppendVSprintf(pFormat, args);
      va_end(args);
   }

   BasicString(const BasicString<CharT>& rhs)
   {
      Reserve(rhs.Size());
      for (int i = 0; i < rhs.Size(); ++i) {
         PushBack(rhs[i]);
      }
   }

   BasicString(BasicString<CharT>&& rhs)
   {
      m_capacity = rhs.m_capacity;
      m_pBegin = rhs.m_pBegin;
      m_pEnd = rhs.m_pEnd;

      rhs.m_capacity = 0;
      rhs.m_pBegin = nullptr;
      rhs.m_pEnd = nullptr;
   }

   BasicString& operator=(const BasicString& rhs)
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

   BasicString& operator=(BasicString&& rhs)
   {
      if (&rhs == this) {
         return *this;
      }

      this->~BasicString();

      m_capacity = rhs.m_capacity;
      m_pBegin = rhs.m_pBegin;
      m_pEnd = rhs.m_pEnd;

      rhs.m_capacity = 0;
      rhs.m_pBegin = nullptr;
      rhs.m_pEnd = nullptr;

      return *this;
   }

   BasicString& operator+=(const CharT* str)
   {
      SizeT strSize = 0;
      while (str[strSize++] != CharT()) {
      }
      --strSize;

      if (Size() + strSize < m_capacity) {
         Reserve(Size() + strSize);
      }

      for (int i = 0; i < strSize; ++i) {
         PushBack(str[i]);
      }

      return *this;
   }

   BasicString& operator+=(const BasicString& str)
   {
      if (Size() + str.Size() < m_capacity) {
         Reserve(Size() + str.Size());
      }

      for (int i = 0; i < str.Size(); ++i) {
         PushBack(str[i]);
      }

      return *this;
   }

   BasicString& operator+=(CharT ch)
   {
      PushBack(ch);

      return *this;
   }

   ~BasicString()
   {
      Clear();

      m_allocator.deallocate(m_pBegin, m_capacity);
   }

   CharT& operator[](SizeT idx)
   {
      return *(m_pBegin + idx);
   }

   const CharT& operator[](SizeT idx) const { return *(m_pBegin + idx); }

   CharT& Front() { return *m_pBegin; }

   const CharT& Front() const { return *m_pBegin; }

   CharT& Back() { return *(m_pEnd - 1); }

   const CharT& Back() const { return *(m_pEnd - 1); }

   CharT* Data() { return m_pBegin; }

   CharT* Data() const { return m_pBegin; }

   const CharT* CStr() const { return m_pBegin; }

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

   void PushBack(CharT ch)
   {
      tryExpandDataStorage();

      m_allocator.construct(m_pEnd++, ch);
   }

   BasicString& AppendVSprintf(const CharT* pFormat, va_list args)
   {
      int formattedStrLen = vsnprintf(nullptr, 0, pFormat, args);
      if (formattedStrLen > 0) {
         int oldSize = Size();
         Resize(oldSize + formattedStrLen);
         vsnprintf(m_pBegin + oldSize, formattedStrLen + 1, pFormat, args);
      }

      return *this;
   }

   BasicString& AppendSprintf(const CharT* pFormat, ...)
   {
      va_list args;
      va_start(args, pFormat);
      AppendVSprintf(pFormat, args);
      va_end(args);

      return *this;
   }

   void Resize(SizeT num)
   {
      if (Size() == num) {
         return;
      }

      if (Size() < num) {
         Reserve(num);
         m_pEnd += num - Size();
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

   Iterator Insert(const Iterator& pos, CharT ch)
   {
      const SizeT idx = pos - m_pBegin;

      tryExpandDataStorage();

      std::memmove(&m_pBegin[idx + 1], &m_pBegin[idx], (Size() - idx) * sizeof(CharT));
      m_allocator.construct(m_pBegin + idx, ch);

      return m_pBegin + idx;
   }

   Iterator Erase(const Iterator& pos)
   {
      m_allocator.destroy(pos);

      --m_pEnd;
      std::memmove(pos, pos + 1, (m_pEnd - pos) * sizeof(CharT));

      return pos;
   }

   SizeT Find(CharT ch, SizeT pos = 0) const
   {
      for (int i = pos; i < Size(); ++i) {
         if (m_pBegin[i] == ch) {
            return static_cast<SizeT>(i);
         }
      }

      return NPos;
   }

   SizeT Find(const CharT* rawStr, SizeT pos = 0) const
   {
      if (rawStr[0] == '\0') {
         return NPos;
      }

      for (int i = pos; i < Size(); ++i) {
         int j = 0;
         while (true) {
            if (rawStr[j] == '\0') {
               return static_cast<SizeT>(i);
            }

            if (i + j >= Size()) {
               return NPos;
            }

            if (m_pBegin[i + j] != rawStr[j]) {
               break;
            }

            ++j;
         }
      }

      return NPos;
   }

   SizeT RFind(CharT ch, SizeT pos = NPos) const
   {
      int i = pos == NPos ? Size() - 1 : pos;
      for (; i >= 0; --i) {
         if (m_pBegin[i] == ch) {
            return static_cast<SizeT>(i);
         }
      }

      return NPos;
   }

   BasicString Substr(SizeT pos = 0, SizeT count = NPos) const
   {
      BasicString str;

      SizeT end = (count == NPos) ? Size() : pos + count;

      if (end > Size()) {
         end = Size();
      }

      str.Reserve(end - pos);

      for (int i = pos; i < end; ++i) {
         str += m_pBegin[i];
      }

      return str;
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
      CharT* newData = m_allocator.allocate(newCapacity + 1);
      std::memset(newData + Size(), CharT(), newCapacity - Size() + 1);
      std::memcpy(newData, m_pBegin, Size() * sizeof(CharT));

      if (m_capacity > 0) {
         m_allocator.deallocate(m_pBegin, m_capacity + 1);
      }

      m_pEnd = newData + Size();
      m_pBegin = newData;
      m_capacity = newCapacity;
   }

private:
   static constexpr SizeT GROW_RATIO = 2;

   CharT* m_pBegin = nullptr;
   CharT* m_pEnd = nullptr;
   SizeT m_capacity = 0;
   std::allocator<CharT> m_allocator;
};

template <typename CharT>
BasicString<CharT> operator+(const BasicString<CharT>& lhs, const BasicString<CharT>& rhs)
{
   BasicString<CharT> ret(lhs);
   ret += rhs;
   return ret;
}

template <typename CharT>
BasicString<CharT> operator+(const BasicString<CharT>& lhs, const CharT* rhs)
{
   BasicString<CharT> ret(lhs);
   ret += rhs;
   return ret;
}

template <typename CharT>
BasicString<CharT> operator+(const CharT* lhs, const BasicString<CharT>& rhs)
{
   BasicString<CharT> ret(lhs);
   ret += rhs;
   return ret;
}

template <typename CharT>
bool operator==(const BasicString<CharT>& lhs, const CharT* rhs)
{
   for (int i = 0; i < lhs.Size(); ++i) {
      if (rhs[i] == CharT() || lhs[i] != rhs[i]) {
         return false;
      }
   }

   return true;
}

template <typename CharT>
bool operator!=(const BasicString<CharT>& lhs, const CharT* rhs)
{
   return !(lhs == rhs);
}

template <typename CharT>
bool operator==(const CharT* lhs, const BasicString<CharT>& rhs)
{
   return rhs == lhs;
}

template <typename CharT>
bool operator!=(const CharT* lhs, const BasicString<CharT>& rhs)
{
   return rhs != lhs;
}

template <typename CharT>
bool operator==(const BasicString<CharT>& lhs, const BasicString<CharT>& rhs)
{
   if (lhs.Size() != rhs.Size()) {
      return false;
   }

   for (int i = 0; i < lhs.Size(); ++i) {
      if (lhs[i] != rhs[i]) {
         return false;
      }
   }

   return true;
}

template <typename CharT>
bool operator!=(const BasicString<CharT>& lhs, const BasicString<CharT>& rhs)
{
   return !(lhs == rhs);
}

template <typename CharT>
auto begin(BasicString<CharT>& arr)
{
   return arr.Begin();
}

template <typename CharT>
auto end(BasicString<CharT>& arr)
{
   return arr.End();
}

template <typename CharT>
auto begin(const BasicString<CharT>& arr)
{
   return arr.CBegin();
}

template <typename CharT>
auto end(const BasicString<CharT>& arr)
{
   return arr.CEnd();
}

/*Hash*/

template <typename CharT>
struct Hash<BasicString<CharT>> {
   SizeT operator()(const BasicString<CharT>& val) const
   {
      return HashRange(val.CBegin(), val.CEnd());
   }
};

/*Input/Output*/

template <typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const BasicString<CharT>& str)
{
   for (int i = 0; i < str.Size(); ++i) {
      os << str[i];
   }

   return os;
}

template <typename CharT>
std::basic_istream<CharT>& Getline(std::basic_istream<CharT>& is, BasicString<CharT>& str)
{
   str.Clear();

   int ioState = std::ios_base::goodbit;

   CharT ch;
   while (is.get(ch) && ch != '\0' && ch != '\n') {
      str += ch;
   }

   if (str.Size() == 0) {
      ioState |= std::ios_base::failbit;
   }

   if (is.rdstate() & std::ios_base::eofbit) {
      ioState |= std::ios_base::eofbit;
   }

   is.clear(ioState);

   return is;
}

using String = BasicString<char>;
using WString = BasicString<wchar_t>;

inline String ToString(int value)
{
   return String(String::CtorSprintf(), "%d", value);
}

inline String ToString(long value)
{
   return String(String::CtorSprintf(), "%ld", value);
}

inline String ToString(long long value)
{
   return String(String::CtorSprintf(), "%lld", value);
}

inline String ToString(unsigned value)
{
   return String(String::CtorSprintf(), "%u", value);
}

inline String ToString(unsigned long value)
{
   return String(String::CtorSprintf(), "%lu", value);
}

inline String ToString(unsigned long long value)
{
   return String(String::CtorSprintf(), "%llu", value);
}

inline String ToString(float value)
{
   return String(String::CtorSprintf(), "%f", value);
}

inline String ToString(double value)
{
   return String(String::CtorSprintf(), "%f", value);
}

inline String ToString(long double value)
{
   return String(String::CtorSprintf(), "%Lf", value);
}

} // namespace BIEngine
