#include "BIGSEventListener.h"

const BIEngine::EventType EvtData_NetPeer_Connected::sk_EventType(0x82037a81);
const BIEngine::EventType EvtData_NetPeer_Disonnected::sk_EventType(0x85d17a31);

void BIRegisterServerEvents()
{
   BIRegisterEvents();
}