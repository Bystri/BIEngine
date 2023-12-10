#pragma once

#include <memory>

#include <pybind11/embed.h>

#include "../../EngineCore/GameApp.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEPhysics3D, m)
{
   py::class_<BIEngine::IGamePhysics3D::RaycastInfo>(m, "RaycastInfo")
      .def(py::init<>())
      .def_readonly("hasHit", &BIEngine::IGamePhysics3D::RaycastInfo::hasHit)
      .def_readonly("hitPosition", &BIEngine::IGamePhysics3D::RaycastInfo::hitPosition)
      .def_readonly("hitActor", &BIEngine::IGamePhysics3D::RaycastInfo::hitActor);

   m.def("Raycast", [](const glm::vec3& from, const glm::vec3& to) {
      return BIEngine::g_pApp->m_pGameLogic->GetGamePhysics3D()->Raycast(from, to);
   });
}