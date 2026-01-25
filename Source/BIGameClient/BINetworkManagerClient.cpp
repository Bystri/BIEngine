#include "BINetworkManagerClient.h"

#include "../../../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

static constexpr float TIME_BETWEEN_HELLOS = 2.f;

static constexpr float TIME_BETWEEN_EVENT_PACKETS = 0.033f;

void BINetworkManagerClient::Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name)
{
   NetworkManager::InitInternal(0);

   m_protocolsManager.AddProtocolReader(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolReader>());
   m_protocolsManager.AddProtocolWriter(BIEngine::MakeShared<EventProtocolWriter>());

   m_pServerPeer = BIEngine::MakeShared<BIEngine::Peer>(0, serverAddress);
   m_peerInfoMap.Emplace(m_pServerPeer->GetId(), m_pServerPeer);

   m_state = NetworkClientState::SayingHello;
   m_timeOfLastHello = 0.0f;
   m_timeOfLastEventPacket = 0.0f;
   m_name = name;
}

void BINetworkManagerClient::Terminate()
{
}

void BINetworkManagerClient::SendOutgoingPackets(const BIEngine::GameTimer& gt)
{
   if (m_state == NetworkClientState::SayingHello) {
      UpdateSayingHello(gt);
      SendMessagesFromQueue(gt);
      return;
   }

   m_protocolsManager.OnBeforePacketsSend(this);
   SendMessagesFromQueue(gt);
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

   BIEngine::Serialize(helloPacket, m_name);

   BIEngine::Logger::WriteMsgLog("Send hello packet to server");
   SendNetworkMessage(*m_pServerPeer, kHelloCC, helloPacket);
}

void BINetworkManagerClient::ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   auto infoItr = m_peerInfoMap.Find(m_pServerPeer->GetId());
   BIEngine::Assert(infoItr != m_peerInfoMap.End(), "Try to process packet from an unknown peer");

   if (infoItr == m_peerInfoMap.End()) {
      return;
   }

   infoItr->second.deliveryNotificationManager.ReadAndProcessState(inputStream);

   if (m_state == NetworkClientState::SayingHello) {
      MessageToRead msg;
      msg.Read(inputStream);

      if (msg.GetId() != 0) {
         return;
      }

      uint32_t packetType;
      BIEngine::Deserialize(msg.GetBuffer(), packetType);

      if (packetType == kWelcomeCC) {
         HandleWelcomePacket(msg.GetBuffer());
      }
   }

   while (inputStream.GetRemainingBitCount() > 32) {
      MessageToRead& msg = infoItr->second.messageQueueToRead.EmplaceBack();
      msg.Read(inputStream);
   }

   Sort(infoItr->second.messageQueueToRead.Begin(), infoItr->second.messageQueueToRead.End());
}

void BINetworkManagerClient::HandleWelcomePacket(BIEngine::InputMemoryBitStream& inputStream)
{
   if (m_state == NetworkClientState::SayingHello) {
      BIEngine::Deserialize(inputStream, m_playerId);
      m_state = NetworkClientState::Welcomed;
      BIEngine::Logger::WriteMsgLog("'%s' was welcomed on client as player %d", m_name.CStr(), m_playerId);

      m_protocolsManager.RegisterPeer(m_pServerPeer);
   }
}
