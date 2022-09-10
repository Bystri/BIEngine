#include "BIGameEvents.h"

const EventType EvtData_StartPaddleMove::sk_EventType(0xf0b5b4fd);
const EventType EvtData_EndPaddleMove::sk_EventType(0x176645ef);
const EventType EvtData_UpdateUI::sk_EventType(0x4a2843d2);

//Регистрация событий внутри скрипта
void RegisterTeapotScriptEvents()
{
    REGISTER_SCRIPT_EVENT(EvtData_StartPaddleMove, EvtData_StartPaddleMove::sk_EventType);
    REGISTER_SCRIPT_EVENT(EvtData_EndPaddleMove, EvtData_EndPaddleMove::sk_EventType);
    REGISTER_SCRIPT_EVENT(EvtData_UpdateUI, EvtData_UpdateUI::sk_EventType);
}
