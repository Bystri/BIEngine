#include "EventNetworkProtocol.h"

const BIEngine::NetworkProtocolType EventProtocolLeader::sk_ProtocolType('EVNT');
const BIEngine::NetworkProtocolType EventProtocolFollower::sk_ProtocolType('EVNT');

/***EventProtocolWriter***/

EventProtocolLeader::EventProtocolLeader()
{
   m_storeEventCommandMoveToDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::StoreEventToForwardDelegate), EvtData_PlayerCommandMoveTo::sk_EventType);
   m_storeEventCommandMoveDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::StoreEventToForwardDelegate), EvtData_Move::sk_EventType);
   m_storeEventCommandTurnDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::StoreEventToForwardDelegate), EvtData_Turn::sk_EventType);
}

EventProtocolLeader::~EventProtocolLeader()
{
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandMoveToDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandMoveDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandTurnDelegateHandler);
}

void EventProtocolLeader::RegisterPeer(uint32_t peerId)
{
   m_peersToSend.PushBack(peerId);
}

void EventProtocolLeader::UnregisterPeer(uint32_t peerId)
{
   const auto itr = BIEngine::Find(m_peersToSend.Begin(), m_peersToSend.End(), peerId);
   if (itr == m_peersToSend.End()) {
      return;
   }

   m_peersToSend.Erase(itr);
}

void EventProtocolLeader::OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager)
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

void EventProtocolLeader::StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData)
{
   m_eventsToSend.PushBack(pEventData);
}

/***EventProtocolReader***/

void EventProtocolFollower::ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream)
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
