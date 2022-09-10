#include "PhysicsEventListener.h"

#include "Physics.h"
#include "../EventManager/Events.h"
#include "../Scripting/LuaStateManager.h"

namespace BIEngine
{

    const EventType EvtData_PhysTrigger_Enter::sk_EventType(0x99358c15);
    const EventType EvtData_PhysTrigger_Leave::sk_EventType(0x3f49c41f);
    const EventType EvtData_PhysCollision::sk_EventType(0x54c58d0d);
    const EventType EvtData_PhysSeparation::sk_EventType(0x3dcea6e1);


    void EvtData_PhysCollision::BuildEventData()
    {
        m_eventData.AssignNewTable(LuaStateManager::Get()->GetLuaState());
        m_eventData.SetInteger("actorA", m_ActorA);
        m_eventData.SetInteger("actorB", m_ActorB);
    }

}

