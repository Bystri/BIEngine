#pragma once

#include <memory>

#include "../../ProcessManager/ProcessManager.h"
#include "../../ProcessManager/DelayProcess.h"
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
   py::class_<BIEngine::Process, std::shared_ptr<BIEngine::Process>>(m, "BaseProcess")
      .def("Succeed", &BIEngine::Process::Succeed)
      .def("Fail", &BIEngine::Process::Fail)
      .def("Pause", &BIEngine::Process::Pause)
      .def("UnPause", &BIEngine::Process::UnPause)

      .def("GetState", &BIEngine::Process::GetState)
      .def("IsAlive", &BIEngine::Process::IsAlive)
      .def("IsDead", &BIEngine::Process::IsDead)
      .def("IsRemoved", &BIEngine::Process::IsRemoved)
      .def("IsPaused", &BIEngine::Process::IsPaused)

      .def("AttachChild", &BIEngine::Process::AttachChild)
      .def("RemoveChild", &BIEngine::Process::RemoveChild)
      .def("PeekChild", &BIEngine::Process::PeekChild);

   py::class_<BIEngine::DelayProcess, BIEngine::Process, std::shared_ptr<BIEngine::DelayProcess>>(m, "DelayProcess")
      .def(py::init<float>());

   py::class_<BIEngine::PyProcess, BIEngine::Process, std::shared_ptr<BIEngine::PyProcess>> processPyClass(m, "Process");

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
      .def(py::init<>());

   m.def("AttachProcess", [](const std::shared_ptr<BIEngine::Process>& pProcess) { BIEngine::ProcessManager::Get()->AttachProcess(pProcess); });
}
