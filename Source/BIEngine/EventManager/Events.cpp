#include "Events.h"

#include "../Physics/Physics2DEventListener.h"
#include "../Physics/Physics3DEventListener.h"

namespace BIEngine {

const EventType EvtData_Actor_Created::sk_EventType(0x23d7aeaa);
const EventType EvtData_Destroy_Actor::sk_EventType(0x1365eb00);
const EventType EvtData_Move_Actor::sk_EventType(0xf0b040b);
const EventType EvtData_Request_Destroy_Actor::sk_EventType(0x76ede0c3);

} // namespace BIEngine
