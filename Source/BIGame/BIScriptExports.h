#pragma once

#include <pybind11/embed.h>

#include "BIEventListener.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIGActionEvent, m)
{
   py::class_<EvtData_Move, BIEngine::BaseEventData, std::shared_ptr<EvtData_Move>>(m, "EvtData_Move")
      .def_readonly_static("eventType", &EvtData_Move::sk_EventType)
      .def("GetPlayerId", &EvtData_Move::GetPlayerId)
      .def("GetDesiredHorizontalAmount", &EvtData_Move::GetDesiredHorizontalAmount)
      .def("GetDesiredVerticalAmount", &EvtData_Move::GetDesiredVerticalAmount);
}
