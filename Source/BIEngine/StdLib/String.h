#pragma once

#include <cstring>
#include <memory>
#include <initializer_list>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

#include "Assert.h"
#include "StdLib.h"
#include "Utility.h"
#include "Iterator.h"

namespace BIEngine {

template <typename CharT>
class BasicString {
   static constexpr SizeT DISABLED_SSO_BIT = 1;

   struct HeapLayout {
      CharT* pBegin = nullptr;
      CharT* pEnd = nullptr;
      SizeT capacity;
   };

   struct SsoLayout {
      static constexpr SizeT CAPACITY = (sizeof(HeapLayout) - sizeof(unsigned char)) / sizeof(CharT);
      CharT buffer[CAPACITY];
      char bufferSize;
   };

   struct CtrlLayout {
      char _pad[sizeof(HeapLayout) - sizeof(unsigned char)];
      unsigned char ctrl;
   };

   struct Rawlayout {
      char rawBuffer[sizeof(HeapLayout)];
   };

   static_assert(sizeof(HeapLayout) >= sizeof(SsoLayout), "heap layout structure must be the greater or same size than sso layout");
   static_assert(sizeof(HeapLayout) == sizeof(CtrlLayout), "heap and ctrl layout structures must be the same size");
   static_assert(sizeof(HeapLayout) == sizeof(Rawlayout), "heap and raw layout structures must be the same size");

   class Layout {
   public:
      Layout()
      {
         std::memset(m_rawLayout.rawBuffer, 0, sizeof(Rawlayout));
      }

      Layout(Layout&& other)
      {
         std::memcpy(m_rawLayout.rawBuffer, other.m_rawLayout.rawBuffer, sizeof(Rawlayout));
         std::memset(other.m_rawLayout.rawBuffer, 0, sizeof(Rawlayout));
      }

      Layout& operator=(Layout&& other)
      {
         if (this == &other) {
            return *this;
         }

         std::memcpy(m_rawLayout.rawBuffer, other.m_rawLayout.rawBuffer, sizeof(Rawlayout));
         std::memset(other.m_rawLayout.rawBuffer, 0, sizeof(Rawlayout));

         return *this;
      }

      CharT* GetBeginPtr()
      {
         return IsHeap() ? m_heapLayout.pBegin : m_ssoLayout.buffer;
      }

      const CharT* GetBeginPtr() const
      {
         return IsHeap() ? m_heapLayout.pBegin : m_ssoLayout.buffer;
      }

      CharT* GetEndPtr()
      {
         return IsHeap() ? m_heapLayout.pEnd : m_ssoLayout.buffer + GetSsoSize();
      }

      const CharT* GetEndPtr() const
      {
         return IsHeap() ? m_heapLayout.pEnd : m_ssoLayout.buffer + GetSsoSize();
      }

      SizeT GetSize() const
      {
         return IsHeap() ? m_heapLayout.pEnd - m_heapLayout.pBegin : GetSsoSize();
      }

      SizeT GetCapacity() const
      {
         return IsHeap() ? GetClassicCapacity() : m_ssoLayout.CAPACITY - 1;
      }

      void IncrementSize(SizeT n = 1)
      {
         if (IsHeap()) {
            m_heapLayout.pEnd += n;
         } else {
            m_ssoLayout.bufferSize += (n << 1);
         }
      }

      void DecrementSize(SizeT n = 1)
      {
         if (IsHeap()) {
            m_heapLayout.pEnd -= n;
         } else {
            m_ssoLayout.bufferSize -= (n << 1);
         }
      }

      bool IsHeap() const
      {
         return m_ctrlLayout.ctrl & DISABLED_SSO_BIT;
      }

      void ExpandDataStorage(SizeT newCapacity)
      {
         ValueType* newData = Allocator().allocate(newCapacity + 1);
         const SizeType size = GetSize();
         std::memset(newData + size, ValueType(), newCapacity - size + 1);

         if (IsHeap()) {
            std::memcpy(newData, m_heapLayout.pBegin, size * sizeof(ValueType));
         } else {
            std::memcpy(newData, m_ssoLayout.buffer, size * sizeof(ValueType));
         }

         if (GetCapacity() > SsoLayout::CAPACITY) {
            Allocator().deallocate(m_heapLayout.pBegin, GetCapacity() + 1);
         }

         m_heapLayout.pEnd = newData + size;
         m_heapLayout.pBegin = newData;
         m_heapLayout.capacity = (newCapacity << 1);
         m_ctrlLayout.ctrl |= DISABLED_SSO_BIT;
      }

   private:
      SizeT GetSsoSize() const
      {
         return (m_ssoLayout.bufferSize >> 1);
      }

      SizeT GetClassicCapacity() const
      {
         SizeT capacity = m_heapLayout.capacity;
         capacity <<= 8;
         capacity >>= 9;
         return capacity;
      }

   private:
      union {
         HeapLayout m_heapLayout;
         SsoLayout m_ssoLayout;
         CtrlLayout m_ctrlLayout;
         Rawlayout m_rawLayout;
      };
   };

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
   ConstPointer Data() const;

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
   void free()
   {
      Clear();

      if (m_layout.IsHeap()) {
         Allocator().deallocate(m_layout.GetBeginPtr(), m_layout.GetCapacity());
      }
   }

   void tryExpandDataStorage();
   void expandDataStorage(SizeType newCapacity);

private:
   static constexpr SizeType GROW_RATIO = 2;

   Layout m_layout;
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
   : m_layout(std::move(rhs.m_layout))
{
}

template <typename CharT>
BasicString<CharT>::~BasicString()
{
   free();
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

   free();

   m_layout = std::move(rhs.m_layout);

   return *this;
}

template <typename CharT>
BasicString<CharT>& BasicString<CharT>::operator+=(const ValueType* str)
{
   SizeType strSize = 0;
   while (str[strSize++] != ValueType()) {
   }
   --strSize;

   if (Size() + strSize < m_layout.GetCapacity()) {
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
   if (Size() + str.Size() < m_layout.GetCapacity()) {
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
   return *(m_layout.GetBeginPtr() + idx);
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::operator[](SizeType idx) const
{
   return *(m_layout.GetBeginPtr() + idx);
}

template <typename CharT>
inline typename BasicString<CharT>::Reference BasicString<CharT>::Front()
{
   return *m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::Front() const
{
   return *m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::Reference BasicString<CharT>::Back()
{
   return *(m_layout.GetEndPtr() - 1);
}

template <typename CharT>
inline typename BasicString<CharT>::ConstReference BasicString<CharT>::Back() const
{
   return *(m_layout.GetEndPtr() - 1);
}

template <typename CharT>
inline typename BasicString<CharT>::Pointer BasicString<CharT>::Data()
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstPointer BasicString<CharT>::Data() const
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline const typename BasicString<CharT>::ValueType* BasicString<CharT>::CStr() const
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::Begin()
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::End()
{
   return m_layout.GetEndPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::Begin() const
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::End() const
{
   return m_layout.GetEndPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::CBegin() const
{
   return m_layout.GetBeginPtr();
}

template <typename CharT>
inline typename BasicString<CharT>::ConstIterator BasicString<CharT>::CEnd() const
{
   return m_layout.GetEndPtr();
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
   return m_layout.GetSize() == 0;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Size() const
{
   return m_layout.GetSize();
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Capacity() const
{
   return m_layout.GetCapacity();
}

template <typename CharT>
inline void BasicString<CharT>::Reserve(SizeType newCap)
{
   if (newCap <= m_layout.GetCapacity()) {
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
      m_layout.IncrementSize(num - Size());
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
   while (Size() > 0) {
      PopBack();
   }
}

template <typename CharT>
inline void BasicString<CharT>::PushBack(ValueType ch)
{
   tryExpandDataStorage();

   *(m_layout.GetEndPtr()) = ch;
   m_layout.IncrementSize();
}

template <typename CharT>
inline BasicString<CharT>& BasicString<CharT>::AppendVSprintf(const ValueType* pFormat, va_list args)
{
   int formattedStrLen = vsnprintf(nullptr, 0, pFormat, args);
   if (formattedStrLen > 0) {
      int oldSize = Size();
      Resize(oldSize + formattedStrLen);
      vsnprintf(m_layout.GetBeginPtr() + oldSize, formattedStrLen + 1, pFormat, args);
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
   const SizeType idx = pos - m_layout.GetBeginPtr();

   tryExpandDataStorage();

   std::memmove(&m_layout.GetBeginPtr()[idx + 1], &m_layout.GetBeginPtr()[idx], (Size() - idx) * sizeof(ValueType));
   m_layout.GetBeginPtr()[idx] = ch;

   return m_layout.GetBeginPtr() + idx;
}

template <typename CharT>
inline void BasicString<CharT>::PopBack()
{
   m_layout.DecrementSize();
}

template <typename CharT>
inline typename BasicString<CharT>::Iterator BasicString<CharT>::Erase(const Iterator& pos)
{
   m_layout.DecrementSize();
   std::memmove(pos, pos + 1, (m_layout.GetEndPtr() - pos) * sizeof(ValueType));

   return pos;
}

template <typename CharT>
inline typename BasicString<CharT>::SizeType BasicString<CharT>::Find(ValueType ch, SizeType pos) const
{
   for (int i = pos; i < Size(); ++i) {
      if (*(m_layout.GetBeginPtr() + i) == ch) {
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

         if (m_layout.GetBeginPtr()[i + j] != rawStr[j]) {
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
      if (m_layout.GetBeginPtr()[i] == ch) {
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
      str += m_layout.GetBeginPtr()[i];
   }

   return str;
}

template <typename CharT>
void BasicString<CharT>::tryExpandDataStorage()
{
   if (Size() == m_layout.GetCapacity()) {
      expandDataStorage(m_layout.GetCapacity() * GROW_RATIO);
   }
}

template <typename CharT>
void BasicString<CharT>::expandDataStorage(SizeType newCapacity)
{
   m_layout.ExpandDataStorage(newCapacity);
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

/*Number conversations*/

inline int Stoi(const String& str)
{
   int num = 0;
   const int retCode = sscanf_s(str.CStr(), "%d", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline long Stol(const String& str)
{
   long num = 0;
   const int retCode = sscanf_s(str.CStr(), "%ld", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline long long Stoll(const String& str)
{
   long long num = 0;
   const int retCode = sscanf_s(str.CStr(), "%lld", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline unsigned long Stoul(const String& str)
{
   unsigned long num = 0;
   const int retCode = sscanf_s(str.CStr(), "%lu", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline unsigned long long Stoull(const String& str)
{
   unsigned long long num = 0;
   const int retCode = sscanf_s(str.CStr(), "%llu", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline float Stof(const String& str)
{
   float num = 0;
   const int retCode = sscanf_s(str.CStr(), "%f", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline double Stod(const String& str)
{
   double num = 0;
   const int retCode = sscanf_s(str.CStr(), "%lf", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

inline long double Stold(const String& str)
{
   long double num = 0;
   const int retCode = sscanf_s(str.CStr(), "%Lf", &num);
   Assert(retCode > 0, "Error occurred while getting number from str: [%s]", str.CStr());
   return num;
}

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
