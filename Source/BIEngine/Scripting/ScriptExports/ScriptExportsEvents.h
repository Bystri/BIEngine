#pragma once

#include <pybind11/embed.h>
#include <pybind11/functional.h>

#include "../../EventManager/Events.h"
#include "../../Physics/Physics2DEventListener.h"
#include "../../Physics/Physics3DEventListener.h"

namespace py = pybind11;

namespace BIEngine {

class ScriptEventListener {
public:
   using EventCallback = std::function<void(const std::shared_ptr<BaseEventData>)>;

   ScriptEventListener(const EventType& eventType, const EventCallback& scriptCallbackFunction);
   ~ScriptEventListener();

   EventListenerDelegate GetDelegate() { return fastdelegate::MakeDelegate(this, &ScriptEventListener::ScriptEventDelegate); }

   EventType GetEventType() const { return m_eventType; }

   void ScriptEventDelegate(IEventDataPtr pEventPtr);

private:
   EventType m_eventType;
   EventCallback m_scriptCallbackFunction;
};

ScriptEventListener::ScriptEventListener(const EventType& eventType, const EventCallback& scriptCallbackFunction)
   : m_scriptCallbackFunction(scriptCallbackFunction)
{
   m_eventType = eventType;
}

ScriptEventListener::~ScriptEventListener()
{
}

// ����� ��������� �������-��������� ������ �������
void ScriptEventListener::ScriptEventDelegate(IEventDataPtr pEvent)
{
   // ����� Python-�������
   std::shared_ptr<BaseEventData> pScriptEvent = std::static_pointer_cast<BaseEventData>(pEvent);

   try {
      m_scriptCallbackFunction(pScriptEvent);
   } catch (py::error_already_set& e) {
      Logger::WriteLog(Logger::LogType::ERROR, e.what());
   } catch (std::runtime_error& e) {
      Logger::WriteLog(Logger::LogType::ERROR, e.what());
   }
}
} // namespace BIEngine

PYBIND11_EMBEDDED_MODULE(BIEEvent, m)
{
   py::class_<BIEngine::BaseEventData, std::shared_ptr<BIEngine::BaseEventData>>(m, "BaseEventData")
      .def(py::init<>())
      .def("GetName", &BIEngine::BaseEventData::GetName)
      .def("GetEventType", &BIEngine::BaseEventData::GetEventType)
      .def("GetTimeStamp", &BIEngine::BaseEventData::GetTimeStamp);

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


   m.def("RegisterEventListener", [](BIEngine::EventType eventType, BIEngine::ScriptEventListener::EventCallback callbackFunction) {
      // ������� C++ ������-���������, ������� ������� �� ��������� �������-��������� ������ �������
      BIEngine::ScriptEventListener* pListener = new BIEngine::ScriptEventListener(eventType, callbackFunction);
      BIEngine::EventManager::Get()->AddListener(pListener->GetDelegate(), eventType);

      uint64_t handle = reinterpret_cast<uint64_t>(pListener);
      return handle;
   });

   m.def("RemoveEventListener", [](uint64_t listenerId) {
      BIEngine::ScriptEventListener* pListener = reinterpret_cast<BIEngine::ScriptEventListener*>(listenerId);
      BIEngine::EventManager::Get()->RemoveListener(pListener->GetDelegate(), pListener->GetEventType());
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
