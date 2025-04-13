#include "BINetworkManagerClient.h"

#include "BIGCEventListener.h"

static constexpr float TIME_BETWEEN_HELLOS = 2.f;

static constexpr float TIME_BETWEEN_EVENT_PACKETS = 0.033f;

void BINetworkManagerClient::Init(const BIEngine::SocketAddress& serverAddress, const std::string& name)
{
   NetworkManager::InitInternal(0);

   m_pReplicationManager = std::make_unique<BIEngine::ObjectReplicationManagerSlave>();

   m_serverAddress = serverAddress;
   m_state = NetworkClientState::SayingHello;
   m_timeOfLastHello = 0.0f;
   m_timeOfLastEventPacket = 0.0f;
   m_name = name;

   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BINetworkManagerClient::StoreEventToForwardDelegate), EvtData_Move::sk_EventType);
}

void BINetworkManagerClient::SendOutgoingPackets(const BIEngine::GameTimer& gt)
{
   switch (m_state) {
      case NetworkClientState::SayingHello:
         UpdateSayingHello(gt);
         break;
      case NetworkClientState::Welcomed:
         UpdateSendingEventPacket(gt);
         break;
   }
}

void BINetworkManagerClient::UpdateSayingHello(const BIEngine::GameTimer& gt)
{
   const float time = gt.TotalTime();

   if (time > m_timeOfLastHello + TIME_BETWEEN_HELLOS) {
      SendHelloPacket();
      m_timeOfLastHello = time;
   }
}

void BINetworkManagerClient::SendHelloPacket()
{
   BIEngine::OutputMemoryBitStream helloPacket;

   BIEngine::Serialize(helloPacket, kHelloCC);
   BIEngine::Serialize(helloPacket, m_name);

   BIEngine::Logger::WriteMsgLog("Send hello packet to server");
   SendPacket(helloPacket, m_serverAddress);
}

void BINetworkManagerClient::ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   BIEngine::Logger::WriteMsgLog("ProcessPacket with byte length %d", inputStream.GetRemainingBitCount());
   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);
   BIEngine::Logger::WriteMsgLog("Packet type %zu", packetType);
   switch (packetType) {
      case kWelcomeCC:
         HandleWelcomePacket(inputStream);
         break;
      case kStateCC:
         HandleStatePacket(inputStream);
         break;
   }
}

void BINetworkManagerClient::HandleWelcomePacket(BIEngine::InputMemoryBitStream& inputStream)
{
   if (m_state == NetworkClientState::SayingHello) {
      BIEngine::Deserialize(inputStream, m_playerId);
      m_state = NetworkClientState::Welcomed;
      BIEngine::Logger::WriteMsgLog("'%s' was welcomed on client as player %d", m_name.c_str(), m_playerId);
   }
}

void BINetworkManagerClient::HandleStatePacket(BIEngine::InputMemoryBitStream& inputStream)
{
   if (m_state == NetworkClientState::Welcomed) {
      //  ReadLastMoveProcessedOnServerTimestamp(inInputStream);

      // tell the replication manager to handle the rest...
      BIEngine::Logger::WriteMsgLog("Got state packet");
      m_pReplicationManager->ProcessPacket(inputStream);
   }
}

void BINetworkManagerClient::StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData)
{
   if (m_state != NetworkClientState::Welcomed) {
      return;
   }

   m_eventsToSend.push_back(pEventData);
}

void BINetworkManagerClient::UpdateSendingEventPacket(const BIEngine::GameTimer& gt)
{
   const float time = gt.TotalTime();

   if (time > m_timeOfLastEventPacket + TIME_BETWEEN_EVENT_PACKETS) {
      SendEventPacket();
      m_timeOfLastEventPacket = time;
   }

   m_eventsToSend.clear();
}

void BINetworkManagerClient::SendEventPacket()
{
   if (m_eventsToSend.empty()) {
      return;
   }

   BIEngine::OutputMemoryBitStream eventPacket;
   BIEngine::Serialize(eventPacket, kEventCC);

   BIEngine::Serialize(eventPacket, m_eventsToSend.size(), 8);

   for (const auto& event : m_eventsToSend) {
      BIEngine::Serialize(eventPacket, event->GetEventType());
      event->Write(eventPacket);
   }

   SendPacket(eventPacket, m_serverAddress);
}
