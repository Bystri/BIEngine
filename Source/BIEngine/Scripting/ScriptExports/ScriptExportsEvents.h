#pragma once

#include <pybind11/embed.h>
#include <pybind11/functional.h>

#include "../../EventManager/Events.h"
#include "../../Physics/Physics2DEventListener.h"
#include "../../Physics/Physics3DEventListener.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEEvent, m)
{
   py::class_<BIEngine::BaseEventData, std::shared_ptr<BIEngine::BaseEventData>>(m, "BaseEventData")
      .def(py::init<>())
      .def("GetName", &BIEngine::BaseEventData::GetName)
      .def("GetEventType", &BIEngine::BaseEventData::GetEventType);

   py::class_<BIEngine::EvtData_Request_Destroy_Actor, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Request_Destroy_Actor>>(m, "EvtData_Request_Destroy_Actor")
      .def("GetActorId", &BIEngine::EvtData_Request_Destroy_Actor::GetActorId);

   py::class_<BIEngine::EvtData_Phys2DCollision, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Phys2DCollision>>(m, "EvtData_Phys2DCollision")
      .def("GetActorA", &BIEngine::EvtData_Phys2DCollision::GetActorA)
      .def("GetActorB", &BIEngine::EvtData_Phys2DCollision::GetActorB)
      .def("GetSumNormalForce", &BIEngine::EvtData_Phys2DCollision::GetSumNormalForce)
      .def("GetSumFrictionForce", &BIEngine::EvtData_Phys2DCollision::GetSumFrictionForce)
      .def("GetCollisionPoints", &BIEngine::EvtData_Phys2DCollision::GetCollisionPoints);

   py::class_<BIEngine::EvtData_Phys3DTrigger_Enter, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Phys3DTrigger_Enter>>(m, "EvtData_Phys3DTrigger_Enter")
      .def_readonly_static("eventType", &BIEngine::EvtData_Phys3DTrigger_Enter::sk_EventType)
      .def("GetTriggerId", &BIEngine::EvtData_Phys3DTrigger_Enter::GetTriggerId)
      .def("GetOtherActor", &BIEngine::EvtData_Phys3DTrigger_Enter::GetOtherActor);

   py::class_<BIEngine::EvtData_Phys3DCollision, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Phys3DCollision>>(m, "EvtData_Phys3DCollision")
      .def("GetActorA", &BIEngine::EvtData_Phys3DCollision::GetActorA)
      .def("GetActorB", &BIEngine::EvtData_Phys3DCollision::GetActorB)
      .def("GetSumNormalForce", &BIEngine::EvtData_Phys3DCollision::GetSumNormalForce)
      .def("GetSumFrictionForce", &BIEngine::EvtData_Phys3DCollision::GetSumFrictionForce)
      .def("GetCollisionPoints", &BIEngine::EvtData_Phys3DCollision::GetCollisionPoints);


   m.def("RegisterEventListener", [](BIEngine::EventType eventType, std::function<void(const std::shared_ptr<BIEngine::BaseEventData>)> callbackFunction) {
      auto scriptListener = [callbackFunction](BIEngine::IEventDataPtr pEvent) {
         std::shared_ptr<BIEngine::BaseEventData> pScriptEvent = std::static_pointer_cast<BIEngine::BaseEventData>(pEvent);

         try {
            callbackFunction(pScriptEvent);
         } catch (py::error_already_set& e) {
            BIEngine::Logger::WriteErrorLog(e.what());
         } catch (std::runtime_error& e) {
            BIEngine::Logger::WriteErrorLog(e.what());
         }
      };

      return BIEngine::EventManager::Get()->AddListener(std::move(scriptListener), eventType);
   });

   m.def("RemoveEventListener", [](uint64_t listenerId) {
      BIEngine::EventManager::Get()->RemoveListener(listenerId);
   });

   m.def("QueueEvent", [](BIEngine::EventType eventType, const BIEngine::IEventDataPtr& pEventData) {
      if (pEventData) {
         BIEngine::EventManager::Get()->QueueEvent(pEventData);
         return true;
      }
      return false;
   });

   m.def("TriggerEvent", [](BIEngine::EventType eventType, const BIEngine::IEventDataPtr& pEventData) {
      if (pEventData)
         return BIEngine::EventManager::Get()->TriggerEvent(pEventData);
      return false;
   });
}
