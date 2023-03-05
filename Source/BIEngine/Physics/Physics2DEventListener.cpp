#include "Physics2DEventListener.h"

#include "Physics2D.h"
#include "../EventManager/Events.h"

namespace BIEngine
{

    const EventType EvtData_Phys2DTrigger_Enter::sk_EventType(0x99358c15);
    const EventType EvtData_Phys2DTrigger_Leave::sk_EventType(0x3f49c41f);
    const EventType EvtData_Phys2DCollision::sk_EventType(0x54c58d0d);
    const EventType EvtData_Phys2DSeparation::sk_EventType(0x3dcea6e1);

}

