#pragma once

#include <pybind11/embed.h>

#include "../../Math/Math.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEMath, m)
{
   m.def("Abs", &BIEngine::Abs<float>);
   m.def("Abs", &BIEngine::Abs<int>);

   m.def("Sqrt", [](float val) { return BIEngine::Sqrt(val); });

   m.def("Clamp", &BIEngine::Clamp<float>);
   m.def("Clamp", &BIEngine::Clamp<int>);

   m.def("MoveTowards", &BIEngine::MoveTowards<float>);
   m.def("SmoothDamp", [](float from, float to, float currentVelocity, float smoothTime, float deltaTime, float maxSpeed) {
      float ret = BIEngine::SmoothDamp(from, to, currentVelocity, smoothTime, deltaTime, maxSpeed);
      return std::tuple<float, float>(ret, currentVelocity);
   });
}
