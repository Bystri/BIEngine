#pragma once

#include <pybind11/embed.h>

#include "../../EngineCore/GameApp.h"
#include "../../Graphics/Scene.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEScene, m)
{
   py::class_<BIEngine::Camera, std::shared_ptr<BIEngine::Camera>>(m, "Camera")
      .def("GetPosition", &BIEngine::Camera::GetPosition)
      .def("GetForward", &BIEngine::Camera::GetForward)
      .def("GetUp", &BIEngine::Camera::GetUp)
      .def("GetRight", &BIEngine::Camera::GetRight)
      .def("GetRotationY", &BIEngine::Camera::GetRotationY)
      .def("GetRotationZ", &BIEngine::Camera::GetRotationZ)

      .def("LookAt", &BIEngine::Camera::LookAt)

      .def("MoveTo", &BIEngine::Camera::MoveTo)
      .def("MoveRelForward", &BIEngine::Camera::MoveRelForward)
      .def("MoveRelSide", &BIEngine::Camera::MoveRelSide)
      .def("SetRotationY", &BIEngine::Camera::SetRotationY)
      .def("SetRotationZ", &BIEngine::Camera::SetRotationZ)
      .def("TurnRelAroundY", &BIEngine::Camera::TurnRelAroundY)
      .def("TurnRelAroundZ", &BIEngine::Camera::TurnRelAroundZ)

      .def("SetFov", &BIEngine::Camera::SetFov)
      .def("GetFov", &BIEngine::Camera::GetFov)
      .def("SetAspectRatio", &BIEngine::Camera::SetAspectRatio)
      .def("GetAspectRatio", &BIEngine::Camera::GetAspectRatio)
      .def("SetNear", &BIEngine::Camera::SetNear)
      .def("GetNear", &BIEngine::Camera::GetNear)
      .def("SetFar", &BIEngine::Camera::SetFar)
      .def("GetFar", &BIEngine::Camera::GetFar);

   m.def("GetCamera", []() {
      return BIEngine::g_pApp->TryGetHumanView(0)->GetScene()->GetCamera();
   });
}
