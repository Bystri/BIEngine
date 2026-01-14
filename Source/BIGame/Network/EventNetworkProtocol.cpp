#include "EventNetworkProtocol.h"

const BIEngine::NetworkProtocolType EventProtocolWriter::sk_ProtocolType('EVNT');
const BIEngine::NetworkProtocolType EventProtocolReader::sk_ProtocolType('EVNT');

/***EventProtocolWriter***/

EventProtocolWriter::EventProtocolWriter()
{
   m_storeEventMoveDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolWriter::StoreEventToForwardDelegate), EvtData_Move::sk_EventType);
   m_storeEventTurnDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolWriter::StoreEventToForwardDelegate), EvtData_Turn::sk_EventType);
   m_storeEventPrimaryAttackDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolWriter::StoreEventToForwardDelegate), EvtData_PrimaryAttack::sk_EventType);
   m_storeEventCommandMoveToDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolWriter::StoreEventToForwardDelegate), EvtData_PlayerCommandMoveTo::sk_EventType);
}

EventProtocolWriter::~EventProtocolWriter()
{
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventMoveDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventTurnDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventPrimaryAttackDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandMoveToDelegateHandler);
}

void EventProtocolWriter::RegisterPeer(BIEngine::PeerPtr pPeer)
{
   m_peersToSend.PushBack(pPeer);
}

void EventProtocolWriter::UnregisterPeer(BIEngine::PeerPtr pPeer)
{
   const auto itr = BIEngine::Find(m_peersToSend.Begin(), m_peersToSend.End(), pPeer);
   if (itr == m_peersToSend.End()) {
      return;
   }

   m_peersToSend.Erase(itr);
}

void EventProtocolWriter::OnBeforePacketsSend(BIEngine::NetworkManager* pNetworkManager)
{
   if (m_eventsToSend.Empty()) {
      return;
   }

   BIEngine::OutputMemoryBitStream eventPacket;

   BIEngine::Serialize(eventPacket, m_eventsToSend.Size(), 8);

   for (const auto& event : m_eventsToSend) {
      BIEngine::Serialize(eventPacket, event->GetEventType());
      event->Write(eventPacket);
   }

   for (auto& pPeer : m_peersToSend) {
      pNetworkManager->SendNetworkMessage(*pPeer, GetType(), eventPacket);
   }

   m_eventsToSend.Clear();
}

void EventProtocolWriter::StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData)
{
   m_eventsToSend.PushBack(pEventData);
}

/***EventProtocolReader***/

void EventProtocolReader::ReceiveMessage(BIEngine::InputMemoryBitStream& inputStream)
{
   uint8_t eventCount = 0;
   BIEngine::Deserialize(inputStream, eventCount);

   while (eventCount > 0) {
      BIEngine::EventType eventType;
      BIEngine::Deserialize(inputStream, eventType);

      BIEngine::IEventDataPtr pEvent = BIEngine::g_eventFactory.Create(eventType);
      pEvent->Read(inputStream);

      BIEngine::EventManager::Get()->QueueEvent(pEvent);

      --eventCount;
   }
}
