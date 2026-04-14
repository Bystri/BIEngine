#pragma once

#include <functional>
#include <utility>

#include "StdLib.h"
#include "MemoryUtils.h"
#include "Utility.h"

namespace BIEngine {

template <typename T>
class EnableSharedFromThis;

template <typename T>
class SharedPtr {
   template <typename T2>
   friend class SharedPtr;

   template <typename T2>
   friend class WeakPtr;

public:
   SharedPtr()
      : m_ptr(nullptr), m_pInfo(new SharedPtrInfo(0))
   {
   }

   template <typename T2>
   explicit SharedPtr(T2* ptr)
      : m_ptr(ptr), m_pInfo(new SharedPtrInfo(1))
   {
      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return;
      }

      DoEnableSharedFromThis(m_pInfo, ptr, ptr);
   }

   template <typename T2, typename Deleter>
   SharedPtr(T2* ptr, Deleter deleter)
      : m_ptr(ptr), m_pInfo(new SharedPtrInfo(1, deleter))
   {
      DoEnableSharedFromThis(m_pInfo, ptr, ptr);
   }

   SharedPtr(std::nullptr_t)
      : m_ptr(nullptr), m_pInfo(new SharedPtrInfo(0))
   {
   }

   template <typename Deleter>
   SharedPtr(std::nullptr_t, Deleter deleter)
      : m_ptr(nullptr), m_pInfo(new SharedPtrInfo(0, deleter))
   {
   }

   SharedPtr(const SharedPtr& rhs)
      : m_ptr(rhs.Get()), m_pInfo(rhs.m_pInfo)
   {
      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->cnt;
   }

   template <typename T2>
   SharedPtr(const SharedPtr<T2>& rhs)
      : m_ptr(rhs.Get()), m_pInfo(reinterpret_cast<SharedPtr::SharedPtrInfo*>(rhs.m_pInfo))
   {
      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->cnt;
   }

   template <typename T2>
   SharedPtr(const SharedPtr<T2>& rhs, T* ptr)
      : m_ptr(ptr), m_pInfo(reinterpret_cast<SharedPtr::SharedPtrInfo*>(rhs.m_pInfo))
   {
      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->cnt;
   }

   SharedPtr(SharedPtr&& rhs)
      : m_ptr(rhs.Get()), m_pInfo(rhs.m_pInfo)
   {
      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;
   }

   template <typename T2>
   SharedPtr(SharedPtr<T2>&& rhs)
      : m_ptr(rhs.Get()), m_pInfo(reinterpret_cast<SharedPtr::SharedPtrInfo*>(rhs.m_pInfo))
   {
      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;
   }

   template <typename T2>
   SharedPtr(SharedPtr<T2>&& rhs, T* ptr)
      : m_ptr(ptr), m_pInfo(reinterpret_cast<SharedPtr::SharedPtrInfo*>(rhs.m_pInfo))
   {
      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;
   }

   template <typename T2>
   SharedPtr(const WeakPtr<T2>& rhs)
      : m_ptr(rhs.m_ptr), m_pInfo(rhs.m_pInfo)
   {
      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->cnt;
   }

   ~SharedPtr()
   {
      Reset();
   }

   SharedPtr& operator=(const SharedPtr& rhs)
   {
      Reset();
      m_ptr = rhs.m_ptr;
      m_pInfo = rhs.m_pInfo;

      if (m_ptr == nullptr) {
         m_pInfo = new SharedPtrInfo(0);
         return *this;
      }
      ++m_pInfo->cnt;

      return *this;
   }

   SharedPtr& operator=(SharedPtr&& rhs)
   {
      if (*this == rhs) {
         return *this;
      }

      Reset();
      m_ptr = rhs.m_ptr;
      m_pInfo = rhs.m_pInfo;

      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;

      return *this;
   }

   explicit operator bool() const
   {
      return m_ptr != nullptr;
   }

   T* Get() const
   {
      return m_ptr;
   }

   T& operator*() const
   {
      return *m_ptr;
   }

   T* operator->() const
   {
      return m_ptr;
   }

   SizeT UseCount() const
   {
      return m_pInfo->cnt;
   }

   void Reset()
   {
      if (m_pInfo == nullptr) {
         return;
      }

      if (m_pInfo->cnt == 1) {
         m_pInfo->deleter(m_ptr);
      }

      if (m_pInfo->cnt > 0) {
         --m_pInfo->cnt;
      }

      if (m_pInfo->cnt == 0 && m_pInfo->weakCnt == 0) {
         delete m_pInfo;
      }

      m_ptr = nullptr;
      m_pInfo = nullptr;
   }

   template <typename T2>
   void Reset(T2* ptr)
   {
      Reset();
      m_ptr = ptr;
      m_pInfo = new SharedPtrInfo(1);
   }

private:
   struct SharedPtrInfo {
      using DeleterType = std::function<void(T*)>;

      SharedPtrInfo(SizeT cnt)
         : cnt(cnt), deleter(DefaultDeleter<T>())
      {
      }

      SharedPtrInfo(SizeT cnt, DeleterType deleter)
         : cnt(cnt), deleter(deleter)
      {
      }

      SizeT cnt = 0;
      SizeT weakCnt = 0;
      DeleterType deleter = DefaultDeleter<T>();
   };

private:
   template <typename U>
   void DoEnableSharedFromThis(const SharedPtrInfo* info, const EnableSharedFromThis<T>* pEnableSharedFromThis, const U* pValue);

   inline void DoEnableSharedFromThis(const SharedPtrInfo*, ...) {}

private:
   T* m_ptr = nullptr;
   SharedPtrInfo* m_pInfo = nullptr;
};

template <typename T>
class EnableSharedFromThis {
public:
   SharedPtr<T> SharedFromThis()
   {
      return SharedPtr<T>(m_weakPtr);
   }

   SharedPtr<const T> SharedFromThis() const
   {
      return SharedPtr<const T>(m_weakPtr);
   }

   WeakPtr<T> WeakFromThis()
   {
      return m_weakPtr;
   }

   WeakPtr<const T> WeakFromThis() const
   {
      return m_weakPtr;
   }

protected:
   template <typename U>
   friend class SharedPtr;

   EnableSharedFromThis() = default;
   ~EnableSharedFromThis() = default;

   EnableSharedFromThis(const EnableSharedFromThis&)
   {
   }

   EnableSharedFromThis& operator=(const EnableSharedFromThis&)
   {
      return *this;
   }

protected:
   mutable WeakPtr<T> m_weakPtr;
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args)
{
   return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename U>
SharedPtr<T> StaticPointerCast(const SharedPtr<U>& ptr)
{
   auto p = static_cast<T*>(ptr.Get());
   return SharedPtr<T>(ptr, p);
}

template <typename T, typename U>
SharedPtr<T> StaticPointerCast(SharedPtr<U>&& ptr)
{
   auto p = static_cast<T*>(ptr.Get());
   return SharedPtr<T>(std::move(ptr), p);
}

template <typename T, typename U>
SharedPtr<T> DynamicPointerCast(const SharedPtr<U>& ptr)
{
   if (auto p = dynamic_cast<T*>(ptr.Get())) {
      return SharedPtr<T>(ptr, p);
   }
   return SharedPtr<T>{};
}

template <typename T, typename U>
SharedPtr<T> DynamicPointerCast(SharedPtr<U>&& ptr)
{
   if (auto p = dynamic_cast<T*>(ptr.Get())) {
      return SharedPtr<T>(std::move(ptr), p);
   }
   return SharedPtr<T>{};
}

template <typename T, typename U>
SharedPtr<T> ConstPointerCast(const SharedPtr<U>& ptr)
{
   auto p = const_cast<T*>(ptr.Get());
   return SharedPtr<T>(ptr, p);
}

template <typename T, typename U>
SharedPtr<T> ConstPointerCast(SharedPtr<U>&& ptr)
{
   auto p = const_cast<T*>(ptr.Get());
   return SharedPtr<T>(std::move(ptr), p);
}

template <typename T, typename U>
SharedPtr<T> ReinterpretPointerCast(const SharedPtr<U>& ptr)
{
   auto p = reinterpret_cast<T*>(ptr.Get());
   return SharedPtr<T>(ptr, p);
}

template <typename T, typename U>
SharedPtr<T> ReinterpretPointerCast(SharedPtr<U>&& ptr)
{
   auto p = reinterpret_cast<T*>(ptr.Get());
   return SharedPtr<T>(std::move(ptr), p);
}

template <typename T1, typename T2>
bool operator==(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return lhs.Get() == rhs.Get();
}

template <typename T1, typename T2>
bool operator!=(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return !(lhs == rhs);
}

template <typename T1, typename T2>
bool operator<(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return lhs.Get() < rhs.Get();
}

template <typename T1, typename T2>
bool operator<=(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return !(rhs < lhs);
}

template <typename T1, typename T2>
bool operator>(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return lhs.Get() > rhs.Get();
}

template <typename T1, typename T2>
bool operator>=(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs)
{
   return !(rhs < lhs);
}

template <typename T>
bool operator==(const SharedPtr<T>& lhs, std::nullptr_t)
{
   return lhs.Get() == nullptr;
}

template <typename T>
bool operator!=(const SharedPtr<T>& lhs, std::nullptr_t)
{
   return !(lhs == nullptr);
}

template <typename T>
bool operator==(std::nullptr_t, const SharedPtr<T>& rhs)
{
   return (rhs == nullptr);
}

template <typename T>
bool operator!=(std::nullptr_t, const SharedPtr<T>& rhs)
{
   return !(nullptr == rhs);
}

template <typename T>
struct Hash<SharedPtr<T>> {
   SizeT operator()(const SharedPtr<T>& val) const
   {
      return Hash<T*>()(val.Get());
   }
};

template <typename T>
class WeakPtr {
   template <typename T2>
   friend class SharedPtr;

public:
   WeakPtr() = default;

   WeakPtr(const WeakPtr& rhs)
      : m_ptr(rhs.m_ptr), m_pInfo(rhs.m_pInfo)
   {
      if (m_ptr == nullptr) {
         m_pInfo = new typename SharedPtr<T>::SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->weakCnt;
   }

   template <typename T2>
   WeakPtr(const WeakPtr<T2>& rhs)
      : m_ptr(rhs.m_ptr), m_pInfo(reinterpret_cast<typename SharedPtr<T>::SharedPtrInfo*>(rhs.m_pInfo))
   {
      if (m_ptr == nullptr) {
         m_pInfo = new typename SharedPtr<T>::SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->weakCnt;
   }

   template <typename T2>
   WeakPtr(const SharedPtr<T2>& rhs)
      : m_ptr(rhs.Get()), m_pInfo(reinterpret_cast<typename SharedPtr<T>::SharedPtrInfo*>(rhs.m_pInfo))
   {
      if (m_ptr == nullptr) {
         m_pInfo = new typename SharedPtr<T>::SharedPtrInfo(0);
         return;
      }
      ++m_pInfo->weakCnt;
   }

   WeakPtr(WeakPtr&& rhs)
      : m_ptr(rhs.m_ptr), m_pInfo(rhs.m_pInfo)
   {
      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;
   }

   template <typename T2>
   WeakPtr(WeakPtr<T2>&& rhs)
      : m_ptr(rhs.m_ptr), m_pInfo(reinterpret_cast<typename SharedPtr<T>::SharedPtrInfo*>(rhs.m_pInfo))
   {
      rhs.m_ptr = nullptr;
      rhs.m_pInfo = nullptr;
   }

   ~WeakPtr()
   {
      Reset();
   }

   WeakPtr& operator=(const WeakPtr& rhs)
   {
      WeakPtr temp(rhs);
      Reset();

      m_ptr = temp.m_ptr;
      m_pInfo = temp.m_pInfo;

      if (m_ptr == nullptr) {
         return *this;
      }

      ++m_pInfo->weakCnt;

      return *this;
   }

   template <typename T2>
   WeakPtr& operator=(const WeakPtr<T2>& rhs)
   {
      WeakPtr<T2> temp(rhs);
      Reset();

      m_ptr = temp.m_ptr;
      m_pInfo = temp.m_pInfo;

      if (m_ptr == nullptr) {
         return *this;
      }

      ++m_pInfo->weakCnt;

      return *this;
   }

   template <typename T2>
   WeakPtr& operator=(const SharedPtr<T2>& rhs)
   {
      WeakPtr<T2> temp(rhs);
      Reset();

      m_ptr = temp.m_ptr;
      m_pInfo = temp.m_pInfo;

      if (m_ptr == nullptr) {
         return *this;
      }

      ++m_pInfo->weakCnt;

      return *this;
   }

   WeakPtr& operator=(WeakPtr&& rhs)
   {
      WeakPtr temp(std::move(rhs));
      Reset();

      m_ptr = temp.m_ptr;
      m_pInfo = temp.m_pInfo;

      if (m_ptr == nullptr) {
         return *this;
      }

      ++m_pInfo->weakCnt;

      return *this;
   }

   template <typename T2>
   WeakPtr& operator=(WeakPtr<T2>&& rhs)
   {
      WeakPtr<T2> temp(std::move(rhs));
      Reset();

      m_ptr = temp.m_ptr;
      m_pInfo = temp.m_pInfo;

      if (m_ptr == nullptr) {
         return *this;
      }

      ++m_pInfo->weakCnt;

      return *this;
   }

   void Reset()
   {
      if (m_pInfo == nullptr) {
         return;
      }

      if (m_pInfo->weakCnt > 0) {
         --m_pInfo->weakCnt;
      }

      if (m_pInfo->cnt == 0 && m_pInfo->weakCnt == 0) {
         delete m_pInfo;
      }

      m_ptr = nullptr;
      m_pInfo = nullptr;
   }

   SharedPtr<T> Lock() const
   {
      if (Expired()) {
         return SharedPtr<T>();
      }

      return SharedPtr<T>(*this);
   }

   bool Expired() const
   {
      return m_pInfo == nullptr || m_pInfo->cnt == 0;
   }

private:
   T* m_ptr = nullptr;
   typename SharedPtr<T>::SharedPtrInfo* m_pInfo = nullptr;
};

template <typename T>
template <typename U>
void typename SharedPtr<T>::DoEnableSharedFromThis(const typename SharedPtr<T>::SharedPtrInfo* info, const typename EnableSharedFromThis<T>* pEnableSharedFromThis, const U* pValue)
{
   if (pEnableSharedFromThis) {
      pEnableSharedFromThis->m_weakPtr = *this;
   }
}

} // namespace BIEngine
