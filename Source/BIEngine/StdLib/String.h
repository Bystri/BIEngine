#pragma once

#include <cstring>
#include <memory>
#include <initializer_list>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

#include "StdLib.h"
#include "Utility.h"
#include "Iterator.h"

namespace BIEngine {

template <typename CharT>
class BasicString {
public:
   using ValueType = CharT;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstReference = const ValueType&;
   using ConstPointer = const ValueType*;
   using SizeType = SizeT;
   using Iterator = ValueType*;
   using ConstIterator = const ValueType*;
   using ReverseIterator = ReverseIterator<Iterator>;
   using ConstReverseIterator = ConstReverseIterator<ConstIterator>;

private:
   using Allocator = std::allocator<ValueType>;

public:
   static const SizeType NPos = static_cast<SizeType>(-1);

   // Stole the idea from EASTL string.h
   struct CtorSprintf {};

   BasicString() = default;
   BasicString(const ValueType* str);
   BasicString(const ValueType* str, SizeType n);
   BasicString(SizeType n);
   BasicString(SizeType n, const ValueType& val);
   BasicString(std::initializer_list<ValueType> l);
   BasicString(CtorSprintf, const ValueType* pFormat, ...);
   BasicString(const BasicString<CharT>& rhs);
   BasicString(BasicString<CharT>&& rhs);

   ~BasicString();

   BasicString& operator=(const BasicString& rhs);
   BasicString& operator=(BasicString&& rhs);
   BasicString& operator+=(const ValueType* str);
   BasicString& operator+=(const BasicString& str);
   BasicString& operator+=(CharT ch);

   Reference operator[](SizeType idx);
   ConstReference operator[](SizeType idx) const;

   Reference Front();
   ConstReference Front() const;

   Reference Back();
   ConstReference Back() const;

   Pointer Data();
   Pointer Data() const;

   const ValueType* CStr() const;

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

   bool Empty() const;
   SizeType Size() const;
   SizeType Capacity() const;

   void Reserve(SizeType newCap);
   void Resize(SizeType num);
   void Clear();

   void PushBack(ValueType ch);

   BasicString& AppendVSprintf(const ValueType* pFormat, va_list args);
   BasicString& AppendSprintf(const ValueType* pFormat, ...);

   Iterator Insert(const Iterator& pos, ValueType ch);

   void PopBack();

   Iterator Erase(const Iterator& pos);

   SizeType Find(ValueType ch, SizeType pos = 0) const;
   SizeType Find(const ValueType* rawStr, SizeType pos = 0) const;
   SizeType RFind(ValueType ch, SizeType pos = NPos) const;

   BasicString Substr(SizeType pos = 0, SizeType count = NPos) const;

private:
   void tryExpandDataStorage();
   void expandDataStorage(SizeType newCapacity);

private:
   static constexpr SizeType GROW_RATIO = 2;

   ValueType* m_pBegin = nullptr;
   ValueType* m_pEnd = nullptr;
   SizeType m_capacity = 0;
};

/*BasicString*/

template <typename CharT>
BasicString<CharT>::BasicString(const ValueType* str)
{
   SizeType strSize = 0;
   while (str[strSize++] != ValueType()) {
   }
   --strSize;

   Reserve(strSize);
   for (int i = 0; i < strSize; ++i) {
      PushBack(str[i]);
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(const ValueType* str, SizeType n)
{
   Reserve(n);
   for (int i = 0; i < n; ++i) {
      PushBack(str[i]);
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(SizeType n)
{
   Reserve(n);
   for (int i = 0; i < n; ++i) {
      PushBack(ValueType());
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(SizeType n, const ValueType& val)
{
   Reserve(n);
   for (int i = 0; i < n; ++i) {
      PushBack(val);
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(std::initializer_list<ValueType> l)
{
   Reserve(l.size());
   for (const auto& val : l) {
      PushBack(val);
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(CtorSprintf, const ValueType* pFormat, ...)
{
   va_list args;
   va_start(args, pFormat);
   AppendVSprintf(pFormat, args);
   va_end(args);
}

template <typename CharT>
BasicString<CharT>::BasicString(const BasicString<CharT>& rhs)
{
   Reserve(rhs.Size());
   for (int i = 0; i < rhs.Size(); ++i) {
      PushBack(rhs[i]);
   }
}

template <typename CharT>
BasicString<CharT>::BasicString(BasicString<CharT>&& rhs)
{
   m_capacity = rhs.m_capacity;
   m_pBegin = rhs.m_pBegin;
   m_pEnd = rhs.m_pEnd;

   rhs.m_capacity = 0;
   rhs.m_pBegin = nullptr;
   rhs.m_pEnd = nullptr;
}

template <typename CharT>
BasicString<CharT>::~BasicString()
{
   Clear();

   Allocator().deallocate(m_pBegin, m_capacity);
}

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator=(const BasicString& rhs)
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

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator=(BasicString&& rhs)
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

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator+=(const ValueType* str)
{
   SizeType strSize = 0;
   while (str[strSize++] != ValueType()) {
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

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator+=(const BasicString& str)
{
   if (Size() + str.Size() < m_capacity) {
      Reserve(Size() + str.Size());
   }

   for (int i = 0; i < str.Size(); ++i) {
      PushBack(str[i]);
   }

   return *this;
}

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator+=(CharT ch)
{
   PushBack(ch);

   return *this;
}

template <typename CharT>
inline typename BasicString<CharT>::Reference BasicString<CharT>::operator[](SizeType idx)
{
   return *(m_pBegin + idx);
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::operator[](SizeType idx) const
{
   return *(m_pBegin + idx);
}

template <typename CharT>
inline typename BasicString<CharT>::Reference BasicString<CharT>::Front()
{
   return *m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::Front() const
{
   return *m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::Reference BasicString<CharT>::Back()
{
   return *(m_pEnd - 1);
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::Back() const
{
   return *(m_pEnd - 1);
}

template <typename CharT>
inline typename BasicString<CharT>::Pointer BasicString<CharT>::Data()
{
   return m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::Pointer BasicString<CharT>::Data() const
{
   return m_pBegin;
}

template <typename CharT>
inline const typename BasicString<CharT>::ValueType* BasicString<CharT>::CStr() const
{
   return m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::Begin()
{
   return m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::End()
{
   return m_pEnd;
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::Begin() const
{
   return m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::End() const
{
   return m_pEnd;
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::CBegin() const
{
   return m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::CEnd() const
{
   return m_pEnd;
}

template <typename CharT>
inline typename BasicString<CharT>::ReverseIterator BasicString<CharT>::RBegin()
{
   return ReverseIterator(End());
}

template <typename CharT>
inline typename BasicString<CharT>::ReverseIterator BasicString<CharT>::REnd()
{
   return ReverseIterator(Begin());
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReverseIterator BasicString<CharT>::RBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReverseIterator BasicString<CharT>::REnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReverseIterator BasicString<CharT>::CRBegin() const
{
   return ConstReverseIterator(CEnd());
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReverseIterator BasicString<CharT>::CREnd() const
{
   return ConstReverseIterator(CBegin());
}

template <typename CharT>
inline bool BasicString<CharT>::Empty() const
{
   return m_pBegin == m_pEnd;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Size() const
{
   return m_pEnd - m_pBegin;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Capacity() const
{
   return m_capacity;
}

template <typename CharT>
inline void BasicString<CharT>::Reserve(SizeType newCap)
{
   if (newCap <= m_capacity) {
      return;
   }

   expandDataStorage(newCap);
}

template <typename CharT>
inline void BasicString<CharT>::Resize(SizeType num)
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

template <typename CharT>
inline void BasicString<CharT>::Clear()
{
   while (m_pEnd != m_pBegin) {
      PopBack();
   }
}

template <typename CharT>
inline void BasicString<CharT>::PushBack(ValueType ch)
{
   tryExpandDataStorage();

   Allocator().construct(m_pEnd++, ch);
}

template <typename CharT>
inline BasicString<CharT>& BasicString<CharT>::AppendVSprintf(const ValueType* pFormat, va_list args)
{
   int formattedStrLen = vsnprintf(nullptr, 0, pFormat, args);
   if (formattedStrLen > 0) {
      int oldSize = Size();
      Resize(oldSize + formattedStrLen);
      vsnprintf(m_pBegin + oldSize, formattedStrLen + 1, pFormat, args);
   }

   return *this;
}

template <typename CharT>
inline BasicString<CharT>& BasicString<CharT>::AppendSprintf(const ValueType* pFormat, ...)
{
   va_list args;
   va_start(args, pFormat);
   AppendVSprintf(pFormat, args);
   va_end(args);

   return *this;
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::Insert(const Iterator& pos, ValueType ch)
{
   const SizeType idx = pos - m_pBegin;

   tryExpandDataStorage();

   std::memmove(&m_pBegin[idx + 1], &m_pBegin[idx], (Size() - idx) * sizeof(ValueType));
   Allocator().construct(m_pBegin + idx, ch);

   return m_pBegin + idx;
}

template <typename CharT>
inline void BasicString<CharT>::PopBack()
{
   Allocator().destroy(--m_pEnd);
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::Erase(const Iterator& pos)
{
   Allocator().destroy(pos);

   --m_pEnd;
   std::memmove(pos, pos + 1, (m_pEnd - pos) * sizeof(ValueType));

   return pos;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Find(ValueType ch, SizeType pos) const
{
   for (int i = pos; i < Size(); ++i) {
      if (m_pBegin[i] == ch) {
         return static_cast<SizeType>(i);
      }
   }

   return NPos;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Find(const ValueType* rawStr, SizeType pos) const
{
   if (rawStr[0] == '\0') {
      return NPos;
   }

   for (int i = pos; i < Size(); ++i) {
      int j = 0;
      while (true) {
         if (rawStr[j] == '\0') {
            return static_cast<SizeType>(i);
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

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::RFind(ValueType ch, SizeType pos) const
{
   int i = pos == NPos ? Size() - 1 : pos;
   for (; i >= 0; --i) {
      if (m_pBegin[i] == ch) {
         return static_cast<SizeType>(i);
      }
   }

   return NPos;
}

template <typename CharT>
inline BasicString<CharT> BasicString<CharT>::Substr(SizeType pos, SizeType count) const
{
   BasicString str;

   SizeType end = (count == NPos) ? Size() : pos + count;

   if (end > Size()) {
      end = Size();
   }

   str.Reserve(end - pos);

   for (int i = pos; i < end; ++i) {
      str += m_pBegin[i];
   }

   return str;
}

template <typename CharT>
void BasicString<CharT>::tryExpandDataStorage()
{
   if (Size() == m_capacity) {
      expandDataStorage(m_capacity == 0 ? 1 : m_capacity * GROW_RATIO);
   }
}

template <typename CharT>
void BasicString<CharT>::expandDataStorage(SizeType newCapacity)
{
   ValueType* newData = Allocator().allocate(newCapacity + 1);
   std::memset(newData + Size(), ValueType(), newCapacity - Size() + 1);
   std::memcpy(newData, m_pBegin, Size() * sizeof(ValueType));

   if (m_capacity > 0) {
      Allocator().deallocate(m_pBegin, m_capacity + 1);
   }

   m_pEnd = newData + Size();
   m_pBegin = newData;
   m_capacity = newCapacity;
}

/*Global operators*/

template <typename CharT>
BasicString<CharT> operator+(const BasicString<CharT>& lhs, const BasicString<CharT>& rhs)
{
   BasicString<CharT> ret(lhs);
   ret += rhs;
   return ret;
}

template <typename CharT>
BasicString<CharT> operator+(const BasicString<CharT>& lhs, const typename BasicString<CharT>::ValueType* rhs)
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
bool operator==(const BasicString<CharT>& lhs, const typename BasicString<CharT>::ValueType* rhs)
{
   for (int i = 0; i < lhs.Size(); ++i) {
      if (rhs[i] == CharT() || lhs[i] != rhs[i]) {
         return false;
      }
   }

   return true;
}

template <typename CharT>
bool operator!=(const BasicString<CharT>& lhs, const typename BasicString<CharT>::ValueType* rhs)
{
   return !(lhs == rhs);
}

template <typename CharT>
bool operator==(const typename BasicString<CharT>::ValueType* lhs, const BasicString<CharT>& rhs)
{
   return rhs == lhs;
}

template <typename CharT>
bool operator!=(const typename BasicString<CharT>::ValueType* lhs, const BasicString<CharT>& rhs)
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

/*foreach loop helpers*/

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

   typename BasicString<CharT>::ValueType ch;
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

/*Using*/

using String = BasicString<char>;
using WString = BasicString<wchar_t>;

/*String global methods*/

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
