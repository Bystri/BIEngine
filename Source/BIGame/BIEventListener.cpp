#include "BIEventListener.h"

const BIEngine::EventType EvtData_Player_Created::sk_EventType(0x2fd2626a);
const BIEngine::EventType EvtData_PlayerActor_Created::sk_EventType(0x526c7bc2);
const BIEngine::EventType EvtData_Move::sk_EventType(0xa29da161);
const BIEngine::EventType EvtData_Turn::sk_EventType(0x23dfd957);
const BIEngine::EventType EvtData_PrimaryAttack::sk_EventType(0x5e84fca9);

void BIRegisterEvents()
{
   BIEngine::g_eventFactory.Register<EvtData_Move>(EvtData_Move::sk_EventType);
   BIEngine::g_eventFactory.Register<EvtData_Turn>(EvtData_Turn::sk_EventType);
   BIEngine::g_eventFactory.Register<EvtData_PrimaryAttack>(EvtData_PrimaryAttack::sk_EventType);
}
