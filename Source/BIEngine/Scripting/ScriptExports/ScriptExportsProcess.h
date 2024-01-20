#pragma once

#include <memory>

#include "../../ProcessManager/ProcessManager.h"
#include "../../Utilities/Logger.h"

namespace BIEngine {

class PyProcess : public Process {
public:
   virtual void OnUpdate(float dt) override
   {
      try {
         /* PYBIND11_OVERRIDE_PURE will acquire the GIL before accessing Python state */
         PYBIND11_OVERRIDE_PURE(
            void,      /* Return type */
            PyProcess, /* Parent class */
            OnUpdate,  /* Name of function */
            dt         /* Argument(s) */
         );
      } catch (std::runtime_error er) {
         Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error: ") + er.what());
         Fail();
      } catch (pybind11::error_already_set er) {
         Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error: ") + er.what());
         Fail();
      }
   }
};

} // namespace BIEngine

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEProcess, m)
{
   py::class_<BIEngine::PyProcess, std::shared_ptr<BIEngine::PyProcess>> processPyClass(m, "Process");

   py::enum_<BIEngine::PyProcess::State>(processPyClass, "State")
      .value("UNINITIALIZED", BIEngine::PyProcess::State::UNINITIALIZED)
      .value("REMOVED", BIEngine::PyProcess::State::REMOVED)
      .value("RUNNING", BIEngine::PyProcess::State::RUNNING)
      .value("PAUSED", BIEngine::PyProcess::State::PAUSED)
      .value("SUCCEEDED", BIEngine::PyProcess::State::SUCCEEDED)
      .value("FAILED", BIEngine::PyProcess::State::FAILED)
      .value("ABORTED", BIEngine::PyProcess::State::ABORTED)
      .export_values();

   processPyClass
      // Конструкторы
      .def(py::init<>()) // Конструктор по умолчанию

      // Биндинг функций и членов класса
      .def("OnUpdate", &BIEngine::PyProcess::OnUpdate)
      .def("Succeed", &BIEngine::PyProcess::Succeed)
      .def("Fail", &BIEngine::PyProcess::Fail)
      .def("Pause", &BIEngine::PyProcess::Pause)
      .def("UnPause", &BIEngine::PyProcess::UnPause)

      .def("GetState", &BIEngine::PyProcess::GetState)
      .def("IsAlive", &BIEngine::PyProcess::IsAlive)
      .def("IsDead", &BIEngine::PyProcess::IsDead)
      .def("IsRemoved", &BIEngine::PyProcess::IsRemoved)
      .def("IsPaused", &BIEngine::PyProcess::IsPaused)

      .def("AttachChild", &BIEngine::PyProcess::AttachChild)
      .def("RemoveChild", &BIEngine::PyProcess::RemoveChild)
      .def("PeekChild", &BIEngine::PyProcess::PeekChild);

   // Глобальная функция привязки процесса
   m.def("AttachProcess", [](const std::shared_ptr<BIEngine::PyProcess>& pProcess) { BIEngine::ProcessManager::Get()->AttachProcess(pProcess); });
}
