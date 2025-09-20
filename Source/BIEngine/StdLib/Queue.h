#pragma once

#include "Deque.h"

namespace BIEngine {

template <typename T, typename Container = Deque<T>>
class Queue {
public:
   using ValueType = typename Container::ValueType;
   using Reference = typename Container::Reference;
   using ConstReference = typename Container::ConstReference;
   using SizeType = typename Container::SizeType;

public:
   bool Empty() const;
   SizeType Size() const;

   void Push(const ValueType& val);
   void Push(ValueType&& val);

   void Pop();

   Reference Front();
   ConstReference Front() const;

   Reference Back();
   ConstReference Back() const;

   template <typename... Args>
   void Emplace(Args&&... args);

private:
   Container m_data;
};

template <typename T, typename Container>
inline bool Queue<T, Container>::Empty() const
{
   return m_data.Empty();
}

template <typename T, typename Container>
inline typename Queue<T, Container>::SizeType Queue<T, Container>::Size() const
{
   return m_data.Size();
}

template <typename T, typename Container>
inline void Queue<T, Container>::Push(const ValueType& val)
{
   m_data.PushBack(val);
}

template <typename T, typename Container>
inline void Queue<T, Container>::Push(ValueType&& val)
{
   m_data.PushBack(std::move(val));
}

template <typename T, typename Container>
inline void Queue<T, Container>::Pop()
{
   m_data.PopFront();
}

template <typename T, typename Container>
inline typename Queue<T, Container>::Reference Queue<T, Container>::Front()
{
   return m_data.Front();
}

template <typename T, typename Container>
inline typename Queue<T, Container>::ConstReference Queue<T, Container>::Front() const
{
   return m_data.Front();
}

template <typename T, typename Container>
inline typename Queue<T, Container>::Reference Queue<T, Container>::Back()
{
   return m_data.Back();
}

template <typename T, typename Container>
inline typename Queue<T, Container>::ConstReference Queue<T, Container>::Back() const
{
   return m_data.Back();
}

template <typename T, typename Container>
template <typename... Args>
inline void Queue<T, Container>::Emplace(Args&&... args)
{
   m_data.EmplaceBack(std::forward<Args>(args)...);
}

} // namespace BIEngine
