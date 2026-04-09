#include "BIEventListener.h"

const BIEngine::EventType EvtData_OnKeyDown::sk_EventType(0x247efb52);
const BIEngine::EventType EvtData_OnKeyUp::sk_EventType(0x87adcf25);
const BIEngine::EventType EvtData_OnPointerMove::sk_EventType(0xaeb0b775);
const BIEngine::EventType EvtData_OnPointerButtonDown::sk_EventType(0x47059a9);
const BIEngine::EventType EvtData_OnPointerButtonUp::sk_EventType(0xcac3f1c1);

const BIEngine::EventType EvtData_Player_Created::sk_EventType(0x2fd2626a);
const BIEngine::EventType EvtData_Player_BeforeDestroyed::sk_EventType(0x54504efb);
const BIEngine::EventType EvtData_PlayerActor_Created::sk_EventType(0x526c7bc2);
const BIEngine::EventType EvtData_Move::sk_EventType(0xa29da161);
const BIEngine::EventType EvtData_Turn::sk_EventType(0x23dfd957);
const BIEngine::EventType EvtData_PrimaryAttack::sk_EventType(0x5e84fca9);
const BIEngine::EventType EvtData_PlayerCommandMoveTo::sk_EventType(0x72e6f4b9);

void BIRegisterEvents()
{
   BIEngine::g_eventFactory.Register<EvtData_Move>(EvtData_Move::sk_EventType);
   BIEngine::g_eventFactory.Register<EvtData_Turn>(EvtData_Turn::sk_EventType);
   BIEngine::g_eventFactory.Register<EvtData_PrimaryAttack>(EvtData_PrimaryAttack::sk_EventType);
   BIEngine::g_eventFactory.Register<EvtData_PlayerCommandMoveTo>(EvtData_PlayerCommandMoveTo::sk_EventType);
}
