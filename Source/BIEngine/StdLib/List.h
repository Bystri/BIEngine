#pragma once

#include <utility>

#include "StdLib.h"

namespace BIEngine {

template <typename T>
class List {
   struct Node {
      Node* prev;
      Node* next;
      T val;
   };

public:
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

   private:
      ConstIterator(const List<T>* pList, Node* pCur)
         : m_pList(pList), m_pCurNode(pCur)
      {
      }

   private:
      const List<T>* m_pList;
      Node* m_pCurNode;
   };

   class Iterator : public ConstIterator {
      friend class List;

   public:
      T& operator*()
      {
         return m_pCurNode->val;
      }

      T* operator->()
      {
         return &m_pCurNode->val;
      }

   private:
      Iterator(const List<T>* pList, Node* pCur)
         : ConstIterator(pList, pCur)
      {
      }
   };

   List() = default;

   explicit List(SizeT n)
   {
      while (n--) {
         EmplaceBack();
      }
   }

   List(const List<T>& rhs)
   {
      for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
         PushBack(*itr);
      }
   }

   List(List<T>&& rhs)
      : m_pFirst(rhs.m_pFirst), m_pLast(rhs.m_pLast), m_size(rhs.m_size)
   {
      rhs.m_pFirst = nullptr;
      rhs.m_pLast = nullptr;
      rhs.m_size = 0;
   }

   List<T>& operator=(const List<T>& rhs)
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

   List<T>& operator=(List<T>&& rhs)
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

   ~List()
   {
      clearImpl();
   }

   bool Empty() const { return m_size == 0; }

   SizeT Size() const { return m_size; }

   T& Front() { return m_pFirst->val; }

   T& Back() { return m_pLast->val; }

   const T& Front() const { return m_pFirst->val; }

   const T& Back() const { return m_pLast->val; }

   Iterator Begin()
   {
      return Iterator(this, m_pFirst);
   }

   Iterator End()
   {
      return Iterator(this, nullptr);
   }

   ConstIterator CBegin() const
   {
      return ConstIterator(this, m_pFirst);
   }

   ConstIterator CEnd() const
   {
      return ConstIterator(this, nullptr);
   }

   void Clear()
   {
      clearImpl();

      m_pFirst = nullptr;
      m_pLast = nullptr;
      m_size = 0;
   }

   void Resize(SizeT num)
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

   void PushBack(const T& val)
   {
      Node* newNode = new Node{m_pLast, nullptr, val};

      pushBackImpl(newNode);
   }

   void PushBack(T&& val)
   {
      Node* newNode = new Node{m_pLast, nullptr, std::forward<T>(val)};

      pushBackImpl(newNode);
   }

   template <typename... U>
   void EmplaceBack(U&&... args)
   {
      Node* newNode = new Node{m_pLast, nullptr, std::forward<U>(args)...};
      pushBackImpl(newNode);
   }

   void PopBack()
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

   void PushFront(const T& val)
   {
      Node* newNode = new Node{nullptr, m_pFirst, val};

      pushFrontImpl(newNode);
   }

   void PushFront(T&& val)
   {
      Node* newNode = new Node{nullptr, m_pFirst, std::forward<T>(val)};

      pushFrontImpl(newNode);
   }

   template <typename... U>
   void EmplaceFront(U&&... args)
   {
      Node* newNode = new Node{nullptr, m_pFirst, std::forward<U>(args)...};
      pushFrontImpl(newNode);
   }

   void PopFront()
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

   Iterator Insert(ConstIterator pos, const T& val)
   {
      Node* newNode = new Node{pos.m_pCurNode->prev, pos.m_pCurNode, val};
      return inserImpl(pos, newNode);
   }

   Iterator Insert(ConstIterator pos, T&& val)
   {
      Node* newNode = new Node{pos.m_pCurNode->prev, pos.m_pCurNode, std::forward(val)};
      return inserImpl(pos, newNode);
   }

   Iterator Erase(ConstIterator pos)
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

   void Remove(const T& val)
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

private:
   void pushBackImpl(Node* newNode)
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

   void pushFrontImpl(Node* newNode)
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

   Iterator inserImpl(ConstIterator pos, Node* newNode)
   {
      pos.m_pCurNode->prev->next = newNode;
      pos.m_pCurNode->prev = newNode;
      ++m_size;

      return Iterator(this, newNode);
   }

   void clearImpl()
   {
      Node* curNode = m_pFirst;
      while (curNode) {
         Node* temp = curNode;
         curNode = curNode->next;
         delete temp;
      }
   }

private:
   Node* m_pFirst = nullptr;
   Node* m_pLast = nullptr;
   SizeT m_size = 0;
};

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

} // namespace BIEngine
