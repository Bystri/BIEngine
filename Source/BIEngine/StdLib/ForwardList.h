#pragma once

#include <utility>

#include "StdLib.h"

namespace BIEngine {

template <typename T>
class ForwardList {
   struct NodeBase {
      NodeBase(NodeBase* nextNode = nullptr)
         : next(nextNode) {}

      NodeBase* next = nullptr;
   };

   struct Node : public NodeBase {
      Node(NodeBase* nextNode, const T& value)
         : NodeBase{nextNode}, val{value}
      {
      }

      Node(NodeBase* nextNode, T&& value)
         : NodeBase{nextNode}, val{std::forward<T>(value)}
      {
      }

      template <typename... U>
      Node(NodeBase* nextNode, U&&... args)
         : NodeBase{nextNode}, val{std::forward<U>(args)...}
      {
      }

      T val;
   };

public:
   class Iterator {
      friend class ForwardList;

   public:
      Iterator& operator++()
      {
         m_pCurNode = m_pCurNode == nullptr ? nullptr : m_pCurNode->next;
         return *this;
      }

      Iterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      T& operator*()
      {
         return static_cast<Node*>(m_pCurNode)->val;
      }

      T* operator->()
      {
         return &static_cast<Node*>(m_pCurNode)->val;
      }

      friend bool operator==(const Iterator& lhs, const Iterator& rhs)
      {
         return lhs.m_pCurNode == rhs.m_pCurNode;
      }

      friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
      {
         return !(lhs == rhs);
      }

   private:
      Iterator(NodeBase* pCur)
         : m_pCurNode(pCur)
      {
      }

   private:
      NodeBase* m_pCurNode;
   };

   class ConstIterator {
      friend class ForwardList;

   public:
      ConstIterator& operator++()
      {
         m_pCurNode = m_pCurNode == nullptr ? nullptr : m_pCurNode->next;
         return *this;
      }

      ConstIterator operator++(int)
      {
         Iterator old = *this;
         operator++();
         return old;
      }

      const T& operator*() const
      {
         return static_cast<Node*>(m_pCurNode)->val;
      }

      const T* operator->() const
      {
         return &static_cast<Node*>(m_pCurNode)->val;
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
      ConstIterator(NodeBase* pCur)
         : m_pCurNode(pCur)
      {
      }


   private:
      NodeBase* m_pCurNode;
   };

   ForwardList() = default;

   explicit ForwardList(SizeT n)
   {
      while (n--) {
         EmplaceFront();
      }
   }

   ForwardList(const ForwardList<T>& rhs)
   {
      auto itrToAdd = CBeforeBegin();
      for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
         itrToAdd = InsertAfter(itrToAdd, *itr);
      }
   }

   ForwardList(ForwardList<T>&& rhs)
      : m_head(rhs.m_head)
   {
      rhs.m_head.next = nullptr;
   }

   ForwardList<T>& operator=(const ForwardList<T>& rhs)
   {
      if (this == &rhs) {
         return *this;
      }

      Clear();

      auto itrToAdd = CBeforeBegin();
      for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
         itrToAdd = InsertAfter(itrToAdd, *itr);
      }

      return *this;
   }

   ForwardList<T>& operator=(ForwardList<T>&& rhs)
   {
      if (this == &rhs) {
         return *this;
      }

      Clear();

      m_head = rhs.m_head;

      rhs.m_head.next = nullptr;

      return *this;
   }

   ~ForwardList()
   {
      clearImpl();
   }

   bool Empty() const { return m_head.next == nullptr; }

   T& Front() { return m_head.next->val; }

   const T& Front() const { return m_head.next->val; }

   Iterator BeforeBegin()
   {
      return Iterator(&m_head);
   }

   Iterator Begin()
   {
      return Iterator(m_head.next);
   }

   Iterator End()
   {
      return Iterator(nullptr);
   }

   Iterator CBeforeBegin()
   {
      return Iterator(&m_head);
   }

   ConstIterator CBegin() const
   {
      return ConstIterator(m_head.next);
   }

   ConstIterator CEnd() const
   {
      return ConstIterator(nullptr);
   }

   void Clear()
   {
      clearImpl();

      m_head.next = nullptr;
   }

   void PushFront(const T& val)
   {
      Node* newNode = new Node{m_head.next, val};

      pushFrontImpl(newNode);
   }

   void PushFront(T&& val)
   {
      Node* newNode = new Node(m_head.next, std::forward<T>(val));

      pushFrontImpl(newNode);
   }

   template <typename... U>
   void EmplaceFront(U&&... args)
   {
      Node* newNode = new Node{m_head.next, std::forward<U>(args)...};
      pushFrontImpl(newNode);
   }

   void PopFront()
   {
      Node* nodeToDelete = static_cast<Node*>(m_head.next);
      m_head.next = m_head.next->next;
      delete nodeToDelete;
   }

   template <typename... U>
   Iterator EmplaceAfter(Iterator pos, U&&... args)
   {
      Node* newNode = new Node{pos.m_pCurNode->next, std::forward<U>(args)...};
      return insertImpl(pos, newNode);
   }

   Iterator InsertAfter(Iterator pos, const T& val)
   {
      Node* newNode = new Node{pos.m_pCurNode->next, val};
      return insertImpl(pos, newNode);
   }

   Iterator InsertAfter(Iterator pos, T&& val)
   {
      Node* newNode = new Node{pos.m_pCurNode->next, std::forward<T>(val)};
      return insertImpl(pos, newNode);
   }

   Iterator EraseAfter(Iterator pos)
   {
      Node* const pNodeToDelete = static_cast<Node*>(pos.m_pCurNode->next);
      pos.m_pCurNode->next = pos.m_pCurNode->next->next;

      Iterator itr(pos.m_pCurNode->next);
      delete pNodeToDelete;
      return itr;
   }

   void SpliceAfter(Iterator pos, const ForwardList<T>& other, Iterator itFromOther)
   {
      Node* const nodeToMove = static_cast<Node*>(itFromOther.m_pCurNode->next);
      itFromOther.m_pCurNode->next = itFromOther.m_pCurNode->next->next;
      nodeToMove->next = pos.m_pCurNode->next;
      pos.m_pCurNode->next = nodeToMove;
   }

private:
   void pushFrontImpl(Node* newNode)
   {
      m_head.next = newNode;
   }

   Iterator insertImpl(Iterator pos, Node* newNode)
   {
      pos.m_pCurNode->next = newNode;

      return Iterator(newNode);
   }

   void clearImpl()
   {
      Node* curNode = static_cast<Node*>(m_head.next);
      while (curNode) {
         Node* temp = curNode;
         curNode = static_cast<Node*>(curNode->next);
         delete temp;
      }
   }

private:
   NodeBase m_head;
};

template <typename T>
auto begin(ForwardList<T>& list)
{
   return list.Begin();
}

template <typename T>
auto end(ForwardList<T>& list)
{
   return list.End();
}

template <typename T>
auto begin(const ForwardList<T>& list)
{
   return list.CBegin();
}

template <typename T>
auto end(const ForwardList<T>& list)
{
   return list.CEnd();
}

} // namespace BIEngine
