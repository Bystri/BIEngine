#pragma once

#include <glm/glm.hpp>
#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include "../EngineCore/Assert.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/String.h"

namespace py = pybind11;

namespace BIEngine {

class PythonStateManager {
public:
   template <typename T>
   class RawPtrWrapper {
   public:
      RawPtrWrapper(T* ptr)
         : m_ptr(ptr)
      {
      }

      T* get() const { return m_ptr; }

   private:
      T* m_ptr;
   };

   template <typename T>
   class SharedPtrWrapper {
   public:
      SharedPtrWrapper(T* ptr)
         : m_ptr(SharedPtr<T>(ptr))
      {
      }

      T* get() const { return m_ptr.Get(); }

   private:
      SharedPtr<T> m_ptr;
   };

public:
   // Инициализация экземпляра одиночки
   static bool Create();
   // Уничтожение экземпляра одиночки
   static void Destroy();

   static PythonStateManager* Get()
   {
      Assert(s_pSingleton, "Call PythonStateManager::Create befor PythonStateManager::Get");
      return s_pSingleton;
   }

   bool Init();
   // Выполняет указанный Python-скрипт
   // Может быть вызван внутри самого скрипта
   void ExecuteFile(const char* resource);
   // Выполняет указанный Python-код
   // Может быть вызван внутри самого скрипта
   void ExecuteString(const char* str);

   py::module_& GetMain();

private:
   // Конструктуры приватны из-за того, что PythonStateManager реализован с помощью паттерна "одиночка"
   PythonStateManager();
   ~PythonStateManager();

private:
   static PythonStateManager* s_pSingleton;
   py::scoped_interpreter* m_pPythonGuard;
   py::module_ m_main;
   py::object m_mainScope;
};

} // namespace BIEngine

namespace pybind11 {
namespace detail {

template <>
struct type_caster<BIEngine::String> : public type_caster_base<BIEngine::String> {
   using base = type_caster_base<BIEngine::String>;

public:
   bool load(handle src, bool convert)
   {
      if (py::isinstance<py::str>(src)) {
         value = new BIEngine::String(py::cast<std::string>(src).c_str());
         return true;
      }
      return false;
   }

   static handle cast(const BIEngine::String& src, return_value_policy policy, handle parent)
   {
      return py::cast(std::string(src.CStr()), policy, parent).release();
   }
};
} // namespace detail
} // namespace pybind11

PYBIND11_DECLARE_HOLDER_TYPE(T, BIEngine::PythonStateManager::RawPtrWrapper<T>);
PYBIND11_DECLARE_HOLDER_TYPE(T, BIEngine::SharedPtr<T>);

// Only needed if the type's `.get()` goes by another name
namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T>
struct holder_helper<BIEngine::SharedPtr<T>> { // <-- specialization

   static const T* get(const BIEngine::SharedPtr<T>& p) { return p.Get(); }
};
} // namespace detail
} // namespace PYBIND11_NAMESPACE
