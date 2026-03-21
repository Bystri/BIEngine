#include "EventNetworkProtocol.h"

const BIEngine::NetworkProtocolType EventProtocolWriter::sk_ProtocolType('EVNT');
const BIEngine::NetworkProtocolType EventProtocolReader::sk_ProtocolType('EVNT');

/***EventProtocolWriter***/

EventProtocolWriter::EventProtocolWriter()
{
   m_storeEventCommandMoveToDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolWriter::StoreEventToForwardDelegate), EvtData_PlayerCommandMoveTo::sk_EventType);
}

EventProtocolWriter::~EventProtocolWriter()
{
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandMoveToDelegateHandler);
}

void EventProtocolWriter::RegisterPeer(uint32_t peerId)
{
   m_peersToSend.PushBack(peerId);
}

void EventProtocolWriter::UnregisterPeer(uint32_t peerId)
{
   const auto itr = BIEngine::Find(m_peersToSend.Begin(), m_peersToSend.End(), peerId);
   if (itr == m_peersToSend.End()) {
      return;
   }

   m_peersToSend.Erase(itr);
}

void EventProtocolWriter::OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager)
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
      pNetworkMessagesManager->SendNetworkMessage(pPeer, GetType(), eventPacket);
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
