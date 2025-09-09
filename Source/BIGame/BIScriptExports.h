#pragma once

#include <pybind11/embed.h>

#include "../BIEngine/Scripting/PythonStateManager.h"

#include "BIEventListener.h"
#include "Locomotion/LocomotionInfoComponent.h"
#include "Combat/CombatStateComponent.h"
#include "Combat/CombatControllerComponent.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIGActionEvent, m)
{
   py::class_<EvtData_Move, BIEngine::BaseEventData, BIEngine::SharedPtr<EvtData_Move>>(m, "EvtData_Move")
      .def_readonly_static("eventType", &EvtData_Move::sk_EventType)
      .def("GetPlayerId", &EvtData_Move::GetPlayerId)
      .def("GetDesiredHorizontalAmount", &EvtData_Move::GetDesiredHorizontalAmount)
      .def("GetDesiredVerticalAmount", &EvtData_Move::GetDesiredVerticalAmount);

   py::class_<EvtData_Turn, BIEngine::BaseEventData, BIEngine::SharedPtr<EvtData_Turn>>(m, "EvtData_Turn")
      .def_readonly_static("eventType", &EvtData_Turn::sk_EventType)
      .def("GetPlayerId", &EvtData_Turn::GetPlayerId)
      .def("GetDesiredDir", &EvtData_Turn::GetDesiredDir);

   py::class_<EvtData_PrimaryAttack, BIEngine::BaseEventData, BIEngine::SharedPtr<EvtData_PrimaryAttack>>(m, "EvtData_PrimaryAttack")
      .def_readonly_static("eventType", &EvtData_PrimaryAttack::sk_EventType)
      .def("GetPlayerId", &EvtData_PrimaryAttack::GetPlayerId);
}

PYBIND11_EMBEDDED_MODULE(BIGActor, m)
{
   py::class_<LocomotionInfoComponent, BIEngine::ActorComponent, BIEngine::PythonStateManager::RawPtrWrapper<LocomotionInfoComponent>>(m, "LocomotionInfoComponent")
      .def("SetCurrentDir", &LocomotionInfoComponent::SetCurrentDir)
      .def("GetCurrentDir", &LocomotionInfoComponent::GetCurrentDir)
      .def("SetCurrentVel", &LocomotionInfoComponent::SetCurrentVel)
      .def("GetCurrentVel", &LocomotionInfoComponent::GetCurrentVel)
      .def("SetInputDir", &LocomotionInfoComponent::SetInputDir)
      .def("GetInputDir", &LocomotionInfoComponent::GetInputDir)
      .def("SetInputVel", &LocomotionInfoComponent::SetInputVel)
      .def("GetInputVel", &LocomotionInfoComponent::GetInputVel);

   py::class_<CombatStateComponent, BIEngine::ActorComponent, BIEngine::PythonStateManager::RawPtrWrapper<CombatStateComponent>>(m, "CombatStateComponent")
      .def("SetIsAttackInProgress", &CombatStateComponent::SetIsAttackInProgress)
      .def("IsAttackInProgress", &CombatStateComponent::IsAttackInProgress);

   py::class_<CombatControllerComponent, BIEngine::ActorComponent, BIEngine::PythonStateManager::RawPtrWrapper<CombatControllerComponent>>(m, "CombatControllerComponent")
      .def("RequestMeleeAttack", &CombatControllerComponent::RequestMeleeAttack);
}
