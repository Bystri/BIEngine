#pragma once

#include <memory>

#include <pybind11/embed.h>

#include "../../Utilities/DebugDraw.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEDebug, m)
{
   m.def("DbgLine", &BIEngine::DebugDraw::Line);
}