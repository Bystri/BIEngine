#pragma once

#include <cstddef>

#include "StdLib.h"

namespace BIEngine {

struct OutputItratorTag {
};

struct InputIteratorTag {
};

struct ForwardIteratorTag : public InputIteratorTag {
};

struct BiderectinalIteratorTag : public ForwardIteratorTag {
};

struct RandomAccessIteratorTag : public BiderectinalIteratorTag {
};

template <typename T>
struct IteratorTraits {
   using IteratorCategory = typename T::IteratorCategory;
   using ValueType = typename T::ValueType;
   using DifferenceType = typename T::DifferenceType;
   using Pointer = typename T::Pointer;
   using Reference = typename T::Reference;
};

template <typename T>
struct IteratorTraits<T*> {
   using IteratorCategory = RandomAccessIteratorTag;
   using ValueType = T;
   using DifferenceType = ptrdiff_t;
   using Pointer = T*;
   using Reference = T&;
};

template <typename Category, typename Type, typename Distance = ptrdiff_t, typename Ptr = Type*, typename Ref = Type&>
struct IteratorBase {
   using IteratorCategory = Category;
   using ValueType = Type;
   using DifferenceType = Distance;
   using Pointer = Ptr;
   using Reference = Ref;
};

template <typename C>
auto Begin(C& c) -> decltype(c.Begin())
{
   return c.Begin();
}

template <typename C>
auto Begin(const C& c) -> decltype(c.Begin())
{
   return c.Begin();
}

template <typename T, SizeT N>
T* Begin(T (&array)[N])
{
   return array;
}

template <typename C>
auto End(C& c) -> decltype(c.End())
{
   return c.End();
}

template <typename C>
auto End(const C& c) -> decltype(c.End())
{
   return c.End();
}

template <typename T, SizeT N>
T* End(T (&array)[N])
{
   return array + N;
}

template <typename C>
auto CBegin(const C& c) -> decltype(Begin(c))
{
   return Begin(c);
}

template <typename C>
auto CEnd(const C& c) -> decltype(End(c))
{
   return End(c);
}

template <typename Itr>
Itr Next(Itr itr)
{
   ++itr;
   return itr;
}

template <typename Itr>
Itr Prev(Itr itr)
{
   --itr;
   return itr;
}

template <typename Itr>
class ConstReverseIterator {
public:
   using IteratorCategory = typename IteratorTraits<Itr>::IteratorCategory;
   using ValueType = typename IteratorTraits<Itr>::ValueType;
   using DifferenceType = typename IteratorTraits<Itr>::DifferenceType;
   using Pointer = typename IteratorTraits<Itr>::Pointer;
   using Reference = typename IteratorTraits<Itr>::Reference;

   ConstReverseIterator(const Itr& pos)
      : m_pos(pos)
   {
   }

   ConstReverseIterator& operator++()
   {
      --m_pos;
      return *this;
   }

   ConstReverseIterator operator++(int)
   {
      ConstReverseIterator old = *this;
      operator++();
      return old;
   }

   ConstReverseIterator& operator--()
   {
      ++m_pos;
      return *this;
   }

   ConstReverseIterator operator--(int)
   {
      ConstReverseIterator old = *this;
      operator--();
      return old;
   }

   typename IteratorTraits<Itr>::Reference operator*()
   {
      return *Prev(m_pos);
   }

   typename IteratorTraits<Itr>::Pointer operator->()
   {
      return Prev(m_pos).operator->();
   }

   friend bool operator==(const ConstReverseIterator& lhs, const ConstReverseIterator& rhs)
   {
      return lhs.m_pos == rhs.m_pos;
   }

   friend bool operator!=(const ConstReverseIterator& lhs, const ConstReverseIterator& rhs)
   {
      return !(lhs == rhs);
   }

protected:
   Itr m_pos;
};

template <typename Itr>
class ReverseIterator : public ConstReverseIterator<Itr> {
public:
   ReverseIterator(const Itr& pos)
      : ConstReverseIterator(pos)
   {
   }

   typename IteratorTraits<Itr>::Reference operator*()
   {
      return *Prev(m_pos);
   }

   typename IteratorTraits<Itr>::Pointer operator->()
   {
      return Prev(m_pos).operator->();
   }
};

} // namespace BIEngine
