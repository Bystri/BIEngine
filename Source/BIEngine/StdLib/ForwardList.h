#pragma once

#include <utility>

#include "StdLib.h"
#include "Utility.h"

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
         : NodeBase{nextNode}, val{std::move(value)}
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
   using ValueType = T;
   using Reference = ValueType&;
   using Pointer = ValueType*;
   using ConstPointer = const ValueType*;
   using ConstReference = const ValueType&;
   using SizeType = SizeT;

   class ConstIterator {
      friend class ForwardList;

   public:
      ConstIterator& operator=(const ConstIterator& rhs)
      {
         m_pCurNode = rhs.m_pCurNode;

         return *this;
      }

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

   protected:
      ConstIterator(const NodeBase* pCur)
         : m_pCurNode(const_cast<NodeBase*>(pCur))
      {
      }

   protected:
      NodeBase* m_pCurNode;
   };

   class Iterator : public ConstIterator {
      friend class ForwardList;

   public:
      T& operator*()
      {
         return static_cast<Node*>(this->m_pCurNode)->val;
      }

      T* operator->()
      {
         return &static_cast<Node*>(this->m_pCurNode)->val;
      }

   protected:
      Iterator(NodeBase* pCur)
         : ConstIterator(pCur)
      {
      }
   };

   ForwardList() = default;
   explicit ForwardList(SizeType n);
   ForwardList(const ForwardList<T>& rhs);
   ForwardList(ForwardList<T>&& rhs);

   ~ForwardList();

   ForwardList<T>& operator=(const ForwardList<T>& rhs);
   ForwardList<T>& operator=(ForwardList<T>&& rhs);

   bool Empty() const;

   void Clear();

   Reference Front();
   ConstReference Front() const;

   Iterator BeforeBegin();
   Iterator Begin();
   Iterator End();

   ConstIterator CBeforeBegin() const;
   ConstIterator CBegin() const;
   ConstIterator CEnd() const;

   void PushFront(const ValueType& val);
   void PushFront(ValueType&& val);

   template <typename... U>
   void EmplaceFront(U&&... args);

   template <typename... U>
   Iterator EmplaceAfter(ConstIterator pos, U&&... args);

   Iterator InsertAfter(ConstIterator pos, const ValueType& val);
   Iterator InsertAfter(ConstIterator pos, ValueType&& val);

   void PopFront();

   Iterator EraseAfter(ConstIterator pos);

   void SpliceAfter(ConstIterator pos, const ForwardList<T>& other, ConstIterator itFromOther);

private:
   void pushFrontImpl(Node* newNode);
   Iterator insertImpl(ConstIterator pos, Node* newNode);
   void clearImpl();

private:
   NodeBase m_head;
};

/*ForwardList*/

template <typename T>
ForwardList<T>::ForwardList(SizeType n)
{
   while (n--) {
      EmplaceFront();
   }
}

template <typename T>
ForwardList<T>::ForwardList(const ForwardList<T>& rhs)
{
   auto itrToAdd = CBeforeBegin();
   for (ConstIterator itr = rhs.CBegin(); itr != rhs.CEnd(); ++itr) {
      itrToAdd = InsertAfter(itrToAdd, *itr);
   }
}

template <typename T>
ForwardList<T>::ForwardList(ForwardList<T>&& rhs)
   : m_head(rhs.m_head)
{
   rhs.m_head.next = nullptr;
}

template <typename T>
ForwardList<T>::~ForwardList()
{
   clearImpl();
}

template <typename T>
ForwardList<T>& ForwardList<T>::operator=(const ForwardList<T>& rhs)
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

template <typename T>
ForwardList<T>& ForwardList<T>::operator=(ForwardList<T>&& rhs)
{
   if (this == &rhs) {
      return *this;
   }

   Clear();

   m_head = rhs.m_head;

   rhs.m_head.next = nullptr;

   return *this;
}

template <typename T>
bool ForwardList<T>::Empty() const
{
   return m_head.next == nullptr;
}

template <typename T>
inline void ForwardList<T>::Clear()
{
   clearImpl();

   m_head.next = nullptr;
}

template <typename T>
inline typename ForwardList<T>::Reference ForwardList<T>::Front()
{
   return m_head.next->val;
}

template <typename T>
inline typename ForwardList<T>::ConstReference ForwardList<T>::Front() const
{
   return m_head.next->val;
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::BeforeBegin()
{
   return Iterator(&m_head);
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::Begin()
{
   return Iterator(m_head.next);
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::End()
{
   return Iterator(nullptr);
}

template <typename T>
inline typename ForwardList<T>::ConstIterator ForwardList<T>::CBeforeBegin() const
{
   return ConstIterator(&m_head);
}

template <typename T>
inline typename ForwardList<T>::ConstIterator ForwardList<T>::CBegin() const
{
   return ConstIterator(m_head.next);
}

template <typename T>
inline typename ForwardList<T>::ConstIterator ForwardList<T>::CEnd() const
{
   return ConstIterator(nullptr);
}

template <typename T>
inline void ForwardList<T>::PushFront(const ValueType& val)
{
   Node* newNode = new Node{m_head.next, val};

   pushFrontImpl(newNode);
}

template <typename T>
inline void ForwardList<T>::PushFront(ValueType&& val)
{
   Node* newNode = new Node(m_head.next, std::move(val));

   pushFrontImpl(newNode);
}

template <typename T>
template <typename... U>
inline void ForwardList<T>::EmplaceFront(U&&... args)
{
   Node* newNode = new Node{m_head.next, std::forward<U>(args)...};
   pushFrontImpl(newNode);
}

template <typename T>
template <typename... U>
inline typename ForwardList<T>::Iterator ForwardList<T>::EmplaceAfter(ConstIterator pos, U&&... args)
{
   Node* newNode = new Node{pos.m_pCurNode->next, std::forward<U>(args)...};
   return insertImpl(pos, newNode);
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::InsertAfter(ConstIterator pos, const ValueType& val)
{
   Node* newNode = new Node{pos.m_pCurNode->next, val};
   return insertImpl(pos, newNode);
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::InsertAfter(ConstIterator pos, ValueType&& val)
{
   Node* newNode = new Node{pos.m_pCurNode->next, std::move<T>(val)};
   return insertImpl(pos, newNode);
}

template <typename T>
inline void ForwardList<T>::PopFront()
{
   Node* nodeToDelete = static_cast<Node*>(m_head.next);
   m_head.next = m_head.next->next;
   delete nodeToDelete;
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::EraseAfter(ConstIterator pos)
{
   Node* const pNodeToDelete = static_cast<Node*>(pos.m_pCurNode->next);
   pos.m_pCurNode->next = pos.m_pCurNode->next->next;

   Iterator itr(pos.m_pCurNode->next);
   delete pNodeToDelete;
   return itr;
}

template <typename T>
inline void ForwardList<T>::SpliceAfter(ConstIterator pos, const ForwardList<T>& other, ConstIterator itFromOther)
{
   Node* const nodeToMove = static_cast<Node*>(itFromOther.m_pCurNode->next);
   itFromOther.m_pCurNode->next = itFromOther.m_pCurNode->next->next;
   nodeToMove->next = pos.m_pCurNode->next;
   pos.m_pCurNode->next = nodeToMove;
}

template <typename T>
inline void ForwardList<T>::pushFrontImpl(Node* newNode)
{
   m_head.next = newNode;
}

template <typename T>
inline typename ForwardList<T>::Iterator ForwardList<T>::insertImpl(ConstIterator pos, Node* newNode)
{
   pos.m_pCurNode->next = newNode;

   return Iterator(newNode);
}

template <typename T>
inline void ForwardList<T>::clearImpl()
{
   Node* curNode = static_cast<Node*>(m_head.next);
   while (curNode) {
      Node* temp = curNode;
      curNode = static_cast<Node*>(curNode->next);
      delete temp;
   }
}

/*foreach loop helpers*/

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

/*Hash*/

template <typename T>
struct Hash<ForwardList<T>> {
   SizeT operator()(const ForwardList<T>& val) const
   {
      return HashRange(val.Begin(), val.End());
   }
};

} // namespace BIEngine
