#include "PythonStateManager.h"

#include <iostream>

#include <pybind11/eval.h>

#include "../Utilities/Logger.h"

namespace BIEngine {

PythonStateManager* PythonStateManager::s_pSingleton = nullptr;

bool PythonStateManager::Create()
{
   assert(s_pSingleton == nullptr);
   if (s_pSingleton) {
      Logger::WriteLog(Logger::LogType::ERROR, "Overwriting PythonStateManager singleton");
      if (s_pSingleton) {
         delete s_pSingleton;
         s_pSingleton = nullptr;
      }
   }

   s_pSingleton = new PythonStateManager();
   if (s_pSingleton)
      return s_pSingleton->Init();

   return false;
}

void PythonStateManager::Destroy()
{
   assert(s_pSingleton);
   if (s_pSingleton) {
      delete s_pSingleton;
      s_pSingleton = nullptr;
   }
}

PythonStateManager::PythonStateManager()
   : m_pPythonGuard(nullptr)
{
}

PythonStateManager::~PythonStateManager()
{
   if (m_pPythonGuard) {
      delete m_pPythonGuard;
      m_pPythonGuard = nullptr;
   }
}

bool PythonStateManager::Init()
{
   m_pPythonGuard = new py::scoped_interpreter();
   if (m_pPythonGuard == nullptr)
      return false;

   m_main = py::module_::import("__main__");
   m_mainScope = m_main.attr("__dict__");

   // Регестрируем функции, которые будут вызываться из скрипта
   m_main.def("ExecuteFile", [this](const char* path) { PythonStateManager::ExecuteFile(path); });
   m_main.def("ExecuteString", [this](const char* str) { PythonStateManager::ExecuteString(str); });

   // TODO: HACK FOR LOADING SCRIPTS. Replace to resource manager
   py::list sysPaths = py::module::import("sys").attr("path");
   sysPaths.append("C:/BystrovI/Projects/BIEngine/Assets/Scripts");
   py::module::import("sys").attr("path") = sysPaths;

   return true;
}

void PythonStateManager::ExecuteFile(const char* path)
{
   try {
      py::eval_file(path, m_mainScope);
   } catch (py::error_already_set& e) {
      Logger::WriteLog(Logger::LogType::ERROR, e.what());
   } catch (std::runtime_error& e) {
      Logger::WriteLog(Logger::LogType::ERROR, e.what());
   }
}

void PythonStateManager::ExecuteString(const char* chunk)
{
   try {
      py::exec(chunk, m_mainScope);
   } catch (py::error_already_set& e) {
      Logger::WriteLog(Logger::LogType::ERROR, e.what());
   }
}

py::module_& PythonStateManager::GetMain()
{
   assert(m_main.is_none() == false);
   return m_main;
}

} // namespace BIEngine

PYBIND11_EMBEDDED_MODULE(BIEVector, m)
{
   py::class_<glm::vec2>(m, "Vec2")
      // Конструкторы
      .def(py::init<>())             // Конструктор по умолчанию
      .def(py::init<float, float>()) // Конструктор вида glm::vec2(float x, float y)

      // Биндинг функций и членов класса
      .def_readwrite("x", &glm::vec2::x)
      .def_readwrite("y", &glm::vec2::y)
      .def("Length", &glm::vec2::length)

      // Перегрузка операторов
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
      .def(py::self -= py::self)
      .def(py::self *= float())
      .def(float() * py::self)
      .def(py::self /= float())
      .def(float() / py::self);

   py::class_<glm::vec3>(m, "Vec3")
      // Конструкторы
      .def(py::init<>())                    // Конструктор по умолчанию
      .def(py::init<float, float, float>()) // Конструктор вида glm::vec3(float x, float y, float z)

      // Биндинг функций и членов класса
      .def_readwrite("x", &glm::vec3::x)
      .def_readwrite("y", &glm::vec3::y)
      .def_readwrite("z", &glm::vec3::z)
      .def("Length", [](const glm::vec3& vec) {
         return glm::length(vec);
      })

      // Перегрузка операторов
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
      .def(py::self -= py::self)
      .def(py::self *= float())
      .def(float() * py::self)
      .def(py::self /= float())
      .def(float() / py::self);
}