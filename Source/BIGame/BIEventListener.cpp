#include "BIEventListener.h"

const BIEngine::EventType EvtData_Player_Created::sk_EventType(0x2fd2626a);
const BIEngine::EventType EvtData_PlayerActor_Created::sk_EventType(0x526c7bc2);
const BIEngine::EventType EvtData_Move::sk_EventType(0xa29da161);

void BIRegisterEvents()
{
   BIEngine::g_eventFactory.Register<EvtData_Move>(EvtData_Move::sk_EventType);
}
