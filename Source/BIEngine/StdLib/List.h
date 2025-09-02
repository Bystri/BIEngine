#pragma once

#include <utility>

#include "StdLib.h"
#include "Utility.h"

namespace BIEngine {

template <typename T>
class List {
   struct Node {
      Node* prev;
      Node* next;
      T val;
   };

public:
   using ValueType = T;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstReference = const ValueType&;
   using ConstPointer = const ValueType*;
   using SizeType = SizeT;

   class ConstIterator {
      friend class List;

   public:
      ConstIterator& operator=(const ConstIterator& rhs)
      {
         m_pList = rhs.m_pList;
         m_pCurNode = rhs.m_pCurNode;

         return *this;
      }

      ConstIterator& operator++()
      {
         m_pCurNode = m_pCurNode == nullptr ? m_pList->m_pFirst : m_pCurNode->next;
         return *this;
      }

      ConstIterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      ConstIterator& operator--()
      {
         m_pCurNode = m_pCurNode == nullptr ? m_pList->m_pLast : m_pCurNode->prev;
         return *this;
      }

      ConstIterator operator--(int)
      {
         Iterator old = *this;
         operator--();
         return old;
      }

      const T& operator*() const
      {
         return m_pCurNode->val;
      }

      const T* operator->() const
      {
         return &m_pCurNode->val;
      }

      friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs)
      {
         return lhs.m_pCurNode == rhs.m_pCurNode;
      }

      friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs)
      {
         return !(lhs == rhs);
      }

   protected:
      ConstIterator(const List<T>* pList, Node* pCur)
         : m_pList(pList), m_pCurNode(pCur)
      {
      }

   protected:
      const List<T>* m_pList;
      Node* m_pCurNode;
   };

   class Iterator : public ConstIterator {
      friend class List;

   public:
      T& operator*()
      {
         return this->m_pCurNode->val;
      }

      T* operator->()
      {
         return &this->m_pCurNode->val;
      }

   protected:
      Iterator(const List<T>* pList, Node* pCur)
         : ConstIterator(pList, pCur)
      {
      }
   };

   List() = default;
   explicit List(SizeType n);
   List(const List<T>& rhs);
   List(List<T>&& rhs);

   ~List();

   List<T>& operator=(const List<T>& rhs);
   List<T>& operator=(List<T>&& rhs);

   bool Empty() const;
   SizeType Size() const;

   Reference Front();
   Reference Back();

   ConstReference Front() const;
   ConstReference Back() const;

   Iterator Begin();
   Iterator End();

   ConstIterator CBegin() const;
   ConstIterator CEnd() const;

   void Clear();

   void Resize(SizeType num);

   void PushBack(const ValueType& val);
   void PushBack(ValueType&& val);

   template <typename... U>
   void EmplaceBack(U&&... args);

   void PopBack();

   void PushFront(const ValueType& val);
   void PushFront(ValueType&& val);

   template <typename... U>
   void EmplaceFront(U&&... args);

   void PopFront();

   Iterator Insert(ConstIterator pos, const ValueType& val);
   Iterator Insert(ConstIterator pos, ValueType&& val);

   Iterator Erase(ConstIterator pos);

   void Remove(const ValueType& val);

private:
   void pushBackImpl(Node* newNode);
   void pushFrontImpl(Node* newNode);
   Iterator inserImpl(ConstIterator pos, Node* newNode);
   void clearImpl();

private:
   Node* m_pFirst = nullptr;
   Node* m_pLast = nullptr;
   SizeType m_size = 0;
};

/*List*/

template <typename T>
List<T>::List(SizeType n)
{
   while (n--) {
      EmplaceBack();
   }
}

template <typename T>
List<T>::List(const List<T>& rhs)
{
   for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
      PushBack(*itr);
   }
}

template <typename T>
List<T>::List(List<T>&& rhs)
   : m_pFirst(rhs.m_pFirst), m_pLast(rhs.m_pLast), m_size(rhs.m_size)
{
   rhs.m_pFirst = nullptr;
   rhs.m_pLast = nullptr;
   rhs.m_size = 0;
}

template <typename T>
List<T>::~List()
{
   clearImpl();
}

template <typename T>
List<T>& List<T>::operator=(const List<T>& rhs)
{
   if (this == &rhs) {
      return *this;
   }

   Clear();

   for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
      PushBack(*itr);
   }

   return *this;
}

template <typename T>
List<T>& List<T>::operator=(List<T>&& rhs)
{
   if (this == &rhs) {
      return *this;
   }

   Clear();

   m_pFirst = rhs.m_pFirst;
   m_pLast = rhs.m_pLast;
   m_size = rhs.m_size;

   rhs.m_pFirst = nullptr;
   rhs.m_pLast = nullptr;
   rhs.m_size = 0;

   return *this;
}

template <typename T>
inline bool List<T>::Empty() const
{
   return m_size == 0;
}

template <typename T>
inline typename List<T>::SizeType List<T>::Size() const
{
   return m_size;
}

template <typename T>
inline typename List<T>::Reference List<T>::Front()
{
   return m_pFirst->val;
}

template <typename T>
inline typename List<T>::Reference List<T>::Back()
{
   return m_pLast->val;
}

template <typename T>
inline typename List<T>::ConstReference List<T>::Front() const
{
   return m_pFirst->val;
}

template <typename T>
inline typename List<T>::ConstReference List<T>::Back() const
{
   return m_pLast->val;
}

template <typename T>
inline typename List<T>::Iterator List<T>::Begin()
{
   return Iterator(this, m_pFirst);
}

template <typename T>
inline typename List<T>::Iterator List<T>::End()
{
   return Iterator(this, nullptr);
}

template <typename T>
inline typename List<T>::ConstIterator List<T>::CBegin() const
{
   return ConstIterator(this, m_pFirst);
}

template <typename T>
inline typename List<T>::ConstIterator List<T>::CEnd() const
{
   return ConstIterator(this, nullptr);
}

template <typename T>
inline void List<T>::Clear()
{
   clearImpl();

   m_pFirst = nullptr;
   m_pLast = nullptr;
   m_size = 0;
}

template <typename T>
inline void List<T>::Resize(SizeType num)
{
   if (num == m_size) {
      return;
   }

   if (num > m_size) {
      for (int i = 0; i < m_size - num; ++i) {
         EmplaceBack();
      }
   } else if (num < m_size) {
      for (int i = 0; i < num - m_size; ++i) {
         PopBack();
      }
   }
}

template <typename T>
inline void List<T>::PushBack(const ValueType& val)
{
   Node* newNode = new Node{m_pLast, nullptr, val};

   pushBackImpl(newNode);
}

template <typename T>
inline void List<T>::PushBack(ValueType&& val)
{
   Node* newNode = new Node{m_pLast, nullptr, std::move(val)};

   pushBackImpl(newNode);
}

template <typename T>
template <typename... U>
inline void List<T>::EmplaceBack(U&&... args)
{
   Node* newNode = new Node{m_pLast, nullptr, std::forward<U>(args)...};
   pushBackImpl(newNode);
}

template <typename T>
inline void List<T>::PopBack()
{
   --m_size;

   Node* nodeToDelete = m_pLast;
   m_pLast = m_pLast->prev;
   delete nodeToDelete;

   if (m_pLast == nullptr) {
      m_pFirst = nullptr;
      return;
   }

   m_pLast->next = nullptr;
}

template <typename T>
inline void List<T>::PushFront(const ValueType& val)
{
   Node* newNode = new Node{nullptr, m_pFirst, val};

   pushFrontImpl(newNode);
}

template <typename T>
inline void List<T>::PushFront(ValueType&& val)
{
   Node* newNode = new Node{nullptr, m_pFirst, std::move(val)};

   pushFrontImpl(newNode);
}

template <typename T>
template <typename... U>
inline void List<T>::EmplaceFront(U&&... args)
{
   Node* newNode = new Node{nullptr, m_pFirst, std::forward<U>(args)...};
   pushFrontImpl(newNode);
}

template <typename T>
inline void List<T>::PopFront()
{
   --m_size;

   Node* nodeToDelete = m_pFirst;
   m_pFirst = m_pFirst->next;
   delete nodeToDelete;

   if (m_pFirst == nullptr) {
      m_pLast = nullptr;
      return;
   }

   m_pFirst->prev = nullptr;
}

template <typename T>
inline typename List<T>::Iterator List<T>::Insert(ConstIterator pos, const ValueType& val)
{
   Node* newNode = new Node{pos.m_pCurNode->prev, pos.m_pCurNode, val};
   return inserImpl(pos, newNode);
}

template <typename T>
inline typename List<T>::Iterator List<T>::Insert(ConstIterator pos, ValueType&& val)
{
   Node* newNode = new Node{pos.m_pCurNode->prev, pos.m_pCurNode, std::move(val)};
   return inserImpl(pos, newNode);
}

template <typename T>
inline typename List<T>::Iterator List<T>::Erase(ConstIterator pos)
{
   --m_size;

   if (pos.m_pCurNode->prev) {
      pos.m_pCurNode->prev->next = pos.m_pCurNode->next;
   } else {
      m_pFirst = pos.m_pCurNode->next;
   }

   if (pos.m_pCurNode->next) {
      pos.m_pCurNode->next->prev = pos.m_pCurNode->prev;
   } else {
      m_pLast = pos.m_pCurNode->prev;
   }

   Iterator itr(this, pos.m_pCurNode->next);
   delete pos.m_pCurNode;
   return itr;
}

template <typename T>
inline void List<T>::Remove(const ValueType& val)
{
   Node* pCurNode = m_pFirst;
   const int initialSize = m_size;
   for (int i = 0; i < initialSize; ++i) {
      if (pCurNode->val != val) {
         pCurNode = pCurNode->next;
         continue;
      }

      Node* const pTemp = pCurNode;
      pCurNode = pCurNode->next;

      if (pTemp->prev) {
         pTemp->prev->next = pTemp->next;
      } else {
         m_pFirst = pTemp->next;
      }

      if (pTemp->next) {
         pTemp->next->prev = pTemp->prev;
      } else {
         m_pLast = pTemp->prev;
      }

      delete pTemp;

      --m_size;
   }
}

template <typename T>
inline void List<T>::pushBackImpl(Node* newNode)
{
   ++m_size;

   if (m_pLast == nullptr) {
      m_pFirst = newNode;
      m_pLast = newNode;
      return;
   }

   m_pLast->next = newNode;
   m_pLast = newNode;
}

template <typename T>
inline void List<T>::pushFrontImpl(Node* newNode)
{
   ++m_size;

   if (m_pFirst == nullptr) {
      m_pFirst = newNode;
      m_pLast = newNode;
      return;
   }

   m_pFirst->prev = newNode;
   m_pFirst = newNode;
}

template <typename T>
inline typename List<T>::Iterator List<T>::inserImpl(ConstIterator pos, Node* newNode)
{
   pos.m_pCurNode->prev->next = newNode;
   pos.m_pCurNode->prev = newNode;
   ++m_size;

   return Iterator(this, newNode);
}

template <typename T>
inline void List<T>::clearImpl()
{
   Node* curNode = m_pFirst;
   while (curNode) {
      Node* temp = curNode;
      curNode = curNode->next;
      delete temp;
   }
}

/*foreach loop helpers*/

template <typename T>
auto begin(List<T>& list)
{
   return list.Begin();
}

template <typename T>
auto end(List<T>& list)
{
   return list.End();
}

template <typename T>
auto begin(const List<T>& list)
{
   return list.CBegin();
}

template <typename T>
auto end(const List<T>& list)
{
   return list.CEnd();
}

/*Hash*/

template <typename T>
struct Hash<List<T>> {
   SizeT operator()(const List<T>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};

} // namespace BIEngine
