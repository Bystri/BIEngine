#include "Events.h"

#include "../Scripting/ScriptEvent.h"
#include "../Physics/PhysicsEventListener.h"

namespace BIEngine
{
	const EventType EvtData_Destroy_Actor::sk_EventType(0x1365eb00);
	const EventType EvtData_Move_Actor::sk_EventType(0xf0b040b);
	const EventType EvtData_New_Render_Component::sk_EventType(0x64016600);
	const EventType EvtData_New_Camera_Component::sk_EventType(0x5db84e6);
	const EventType EvtData_Request_Destroy_Actor::sk_EventType(0x76ede0c3);

	void RegisterEngineScriptEvents()
	{
		REGISTER_SCRIPT_EVENT(EvtData_Request_Destroy_Actor, EvtData_Request_Destroy_Actor::sk_EventType);
		REGISTER_SCRIPT_EVENT(EvtData_PhysCollision, EvtData_PhysCollision::sk_EventType);
	}

}



