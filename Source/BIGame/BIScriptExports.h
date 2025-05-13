#pragma once

#include <pybind11/embed.h>

#include "BIEventListener.h"
#include "Locomotion/LocomotionInfoComponent.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIGActionEvent, m)
{
   py::class_<EvtData_Move, BIEngine::BaseEventData, std::shared_ptr<EvtData_Move>>(m, "EvtData_Move")
      .def_readonly_static("eventType", &EvtData_Move::sk_EventType)
      .def("GetPlayerId", &EvtData_Move::GetPlayerId)
      .def("GetDesiredHorizontalAmount", &EvtData_Move::GetDesiredHorizontalAmount)
      .def("GetDesiredVerticalAmount", &EvtData_Move::GetDesiredVerticalAmount);

   py::class_<EvtData_Turn, BIEngine::BaseEventData, std::shared_ptr<EvtData_Turn>>(m, "EvtData_Turn")
      .def_readonly_static("eventType", &EvtData_Turn::sk_EventType)
      .def("GetPlayerId", &EvtData_Turn::GetPlayerId)
      .def("GetDesiredDir", &EvtData_Turn::GetDesiredDir);
}

PYBIND11_EMBEDDED_MODULE(BIGActor, m)
{
   py::class_<LocomotionInfoComponent, BIEngine::ActorComponent, std::shared_ptr<LocomotionInfoComponent>>(m, "LocomotionInfoComponent")
      .def("SetCurrentDir", &LocomotionInfoComponent::SetCurrentDir)
      .def("GetCurrentDir", &LocomotionInfoComponent::GetCurrentDir)
      .def("SetCurrentVel", &LocomotionInfoComponent::SetCurrentVel)
      .def("GetCurrentVel", &LocomotionInfoComponent::GetCurrentVel)
      .def("SetInputDir", &LocomotionInfoComponent::SetInputDir)
      .def("GetInputDir", &LocomotionInfoComponent::GetInputDir)
      .def("SetInputVel", &LocomotionInfoComponent::SetInputVel)
      .def("GetInputVel", &LocomotionInfoComponent::GetInputVel);
}
