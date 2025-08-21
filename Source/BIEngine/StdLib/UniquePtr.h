#pragma once

#include <utility>

#include "../StdLib/StdLib.h"
#include "MemoryUtils.h"

namespace BIEngine {

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:
   UniquePtr() = default;

   explicit UniquePtr(T* ptr)
      : m_ptr(ptr)
   {
   }

   template <typename T2, typename D2>
   UniquePtr(UniquePtr<T2, D2>&& rhs)
      : m_ptr(rhs.Get())
   {
      rhs.Release();
   }

   ~UniquePtr()
   {
      if (m_ptr == nullptr) {
         return;
      }

      Deleter()(m_ptr);
   }

   UniquePtr& operator=(UniquePtr&& rhs)
   {
      if (*this == rhs) {
         return *this;
      }

      if (m_ptr) {
         Deleter()(m_ptr);
      }

      m_ptr = rhs.m_ptr;
      rhs.m_ptr = nullptr;

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

   T* Release()
   {
      T* temp = m_ptr;
      m_ptr = nullptr;

      return temp;
   }

   void Reset(T* ptr)
   {
      T* temp = m_ptr;
      m_ptr = ptr;
      if (temp) {
         Deleter()(temp);
      }
   }

private:
   UniquePtr(const UniquePtr&) = delete;
   UniquePtr& operator=(const UniquePtr&) = delete;

private:
   T* m_ptr = nullptr;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
   UniquePtr() = default;

   explicit UniquePtr(T* ptr)
      : m_ptr(ptr)
   {
   }

   template <typename T2, typename D2>
   UniquePtr(UniquePtr<T2, D2>&& rhs)
      : m_ptr(rhs.Get())
   {
      rhs.Release();
   }

   ~UniquePtr()
   {
      if (m_ptr == nullptr) {
         return;
      }

      Deleter()(m_ptr);
   }

   UniquePtr& operator=(UniquePtr&& rhs)
   {
      if (*this == rhs) {
         return *this;
      }

      if (m_ptr) {
         Deleter()(m_ptr);
      }

      m_ptr = rhs.m_ptr;
      rhs.m_ptr = nullptr;

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

   T& operator[](SizeT idx) const
   {
      return *(m_ptr + idx);
   }

   T* Release()
   {
      T* temp = m_ptr;
      m_ptr = nullptr;

      return temp;
   }

   void Reset(T* ptr)
   {
      T* temp = m_ptr;
      m_ptr = ptr;
      if (temp) {
         Deleter()(temp);
      }
   }

private:
   UniquePtr(const UniquePtr&) = delete;
   UniquePtr& operator=(const UniquePtr&) = delete;

private:
   T* m_ptr = nullptr;
};

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
   return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator==(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return lhs.Get() == rhs.Get();
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator!=(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return !(lhs == rhs);
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator<(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return lhs.Get() < rhs.Get();
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator<=(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return !(rhs < lhs);
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator>(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return lhs.Get() > rhs.Get();
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator>=(const UniquePtr<T1, D1>& lhs, const UniquePtr<T2, D2>& rhs)
{
   return !(rhs < lhs);
}

template <typename T, typename D>
bool operator==(const UniquePtr<T, D>& lhs, std::nullptr_t)
{
   return lhs.Get() == nullptr;
}

template <typename T, typename D>
bool operator!=(const UniquePtr<T, D>& lhs, std::nullptr_t)
{
   return !(lhs == nullptr);
}

template <typename T, typename D>
bool operator==(std::nullptr_t, const UniquePtr<T, D>& rhs)
{
   return (rhs == nullptr);
}

template <typename T, typename D>
bool operator!=(std::nullptr_t, const UniquePtr<T, D>& rhs)
{
   return !(nullptr == rhs);
}

} // namespace BIEngine
