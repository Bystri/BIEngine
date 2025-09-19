#pragma once

#include "Deque.h"

namespace BIEngine {

template <typename T, typename Container = Deque<T>>
class Stack {
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
   Reference Top();
   ConstReference Top() const;

   template <typename... Args>
   void Emplace(Args&&... args);

private:
   Container m_data;
};

template <typename T, typename Container>
inline bool Stack<T, Container>::Empty() const
{
   return m_data.Empty();
}

template <typename T, typename Container>
inline typename Stack<T, Container>::SizeType Stack<T, Container>::Size() const
{
   return m_data.Size();
}

template <typename T, typename Container>
inline void Stack<T, Container>::Push(const ValueType& val)
{
   m_data.PushBack(val);
}

template <typename T, typename Container>
inline void Stack<T, Container>::Push(ValueType&& val)
{
   m_data.PushBack(std::move(val));
}

template <typename T, typename Container>
inline void Stack<T, Container>::Pop()
{
   m_data.PopBack();
}

template <typename T, typename Container>
inline typename Stack<T, Container>::Reference Stack<T, Container>::Top()
{
   return m_data.Back();
}

template <typename T, typename Container>
inline typename Stack<T, Container>::ConstReference Stack<T, Container>::Top() const
{
   return m_data.Back();
}

template <typename T, typename Container>
template <typename... Args>
inline void Stack<T, Container>::Emplace(Args&&... args)
{
   m_data.EmplaceBack(std::forward<Args>(args)...);
}

} // namespace BIEngine
