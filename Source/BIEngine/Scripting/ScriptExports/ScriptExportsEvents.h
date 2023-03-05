#pragma once

#include <pybind11/embed.h>

#include "../../EventManager/Events.h"
#include "../../Physics/Physics2DEventListener.h"
#include "../../Physics/Physics3DEventListener.h"

namespace py = pybind11;

namespace BIEngine
{

	class ScriptEventListener
	{
	public:
		using EventCallback = std::function<void(const std::shared_ptr<BaseEventData>&)>;

		explicit ScriptEventListener(const EventType& eventType, const EventCallback& scriptCallbackFunction);
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

	//Вызов настоящей функции-слушателя внутри скрипта
	void ScriptEventListener::ScriptEventDelegate(IEventDataPtr pEvent)
	{
		//Вызов Python-функции
		std::shared_ptr<BaseEventData> pScriptEvent = std::static_pointer_cast<BaseEventData>(pEvent);
		m_scriptCallbackFunction(pScriptEvent);
	}
}


PYBIND11_EMBEDDED_MODULE(BIEEvent, m)
{
    py::class_<BIEngine::BaseEventData, std::shared_ptr<BIEngine::BaseEventData>>(m, "BaseEventData")
		.def(py::init<>())
		.def("GetName", &BIEngine::BaseEventData::GetName)
		.def("GetEventType", &BIEngine::BaseEventData::GetEventType)
		.def("GetTimeStamp", &BIEngine::BaseEventData::GetTimeStamp);

    py::class_<BIEngine::EvtData_Request_Destroy_Actor, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Request_Destroy_Actor>>(m, "EvtData_Request_Destroy_Actor")
        .def("GetActorId", &BIEngine::EvtData_Request_Destroy_Actor::GetActorId);

    py::class_<BIEngine::EvtData_Phys2DCollision, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Phys2DCollision>>(m, "EvtData_PhysCollision")
        .def("GetActorA", &BIEngine::EvtData_Phys2DCollision::GetActorA)
		.def("GetActorB", &BIEngine::EvtData_Phys2DCollision::GetActorB)
		.def("GetSumNormalForce", &BIEngine::EvtData_Phys2DCollision::GetSumNormalForce)
		.def("GetSumFrictionForce", &BIEngine::EvtData_Phys2DCollision::GetSumFrictionForce)
		.def("GetCollisionPoints", &BIEngine::EvtData_Phys2DCollision::GetCollisionPoints);

	py::class_<BIEngine::EvtData_Phys3DCollision, BIEngine::BaseEventData, std::shared_ptr<BIEngine::EvtData_Phys3DCollision>>(m, "EvtData_PhysCollision")
		.def("GetActorA", &BIEngine::EvtData_Phys3DCollision::GetActorA)
		.def("GetActorB", &BIEngine::EvtData_Phys3DCollision::GetActorB)
		.def("GetSumNormalForce", &BIEngine::EvtData_Phys3DCollision::GetSumNormalForce)
		.def("GetSumFrictionForce", &BIEngine::EvtData_Phys3DCollision::GetSumFrictionForce)
		.def("GetCollisionPoints", &BIEngine::EvtData_Phys3DCollision::GetCollisionPoints);

	
	m.def("RegisterEventListener", [](BIEngine::EventType eventType, BIEngine::ScriptEventListener::EventCallback callbackFunction)
	{
		//Создаем C++ прокси-слушателя, который ссылает на настоящую функцию-слушателя внутри скрипта
		BIEngine::ScriptEventListener* pListener = new BIEngine::ScriptEventListener(eventType, callbackFunction);
		BIEngine::EventManager::Get()->AddListener(pListener->GetDelegate(), eventType);

		unsigned long handle = reinterpret_cast<unsigned long>(pListener);
		return handle;
	});

	m.def("RemoveEventListener", [](unsigned long listenerId)
	{
		assert(listenerId != 0);

		BIEngine::ScriptEventListener* pListener = reinterpret_cast<BIEngine::ScriptEventListener*>(listenerId);
		BIEngine::EventManager::Get()->RemoveListener(pListener->GetDelegate(), pListener->GetEventType());
	});

	m.def("QueueEvent", [](BIEngine::EventType eventType, const BIEngine::IEventDataPtr& pEventData)
	{
		if (pEventData)
		{
			BIEngine::EventManager::Get()->QueueEvent(pEventData);
			return true;
		}
		return false;
	});

	m.def("TriggerEvent", [](BIEngine::EventType eventType, const BIEngine::IEventDataPtr& pEventData)
	{
		if (pEventData)
			return BIEngine::EventManager::Get()->TriggerEvent(pEventData);
		return false;
	});
}