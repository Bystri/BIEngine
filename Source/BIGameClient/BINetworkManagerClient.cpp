#include "BINetworkManagerClient.h"

#include "../../../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

static constexpr float TIME_BETWEEN_HELLOS = 2.f;

static constexpr float TIME_BETWEEN_EVENT_PACKETS = 0.033f;

bool BINetworkManagerClient::Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name)
{
   m_networkMessagesManager.AddProtocolReader(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolReader>());
   m_networkMessagesManager.AddProtocolWriter(BIEngine::MakeShared<EventProtocolWriter>());
   m_pServerPeer = BIEngine::MakeShared<BIEngine::Peer>(0, serverAddress);
   m_name = name;

   return m_networkClient.Init(serverAddress);
}

void BINetworkManagerClient::Terminate()
{
}

void BINetworkManagerClient::Update(const BIEngine::GameTimer& gt)
{
   m_networkClient.Update(gt.TotalTime());

   if (m_networkClient.GetState() == BIEngine::NetworkClient::State::Welcomed) {
      // TODO: Temp
      if (m_peerInfoMap.Empty()) {
         m_peerInfoMap.Emplace(m_pServerPeer->GetId(), m_pServerPeer);
         m_networkMessagesManager.RegisterPeer(
            m_pServerPeer->GetId(), 
             gt, 
             std::bind(&BIEngine::NetworkClient::SendPacket, m_networkClient, std::placeholders::_1));
      }
   }

   while (true) {
      BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pPacketData = m_networkClient.ReceivePacket();
      if (pPacketData == nullptr) {
         break;
      }

      m_networkMessagesManager.ProcessPacket(m_pServerPeer->GetId(), *pPacketData, gt);
   }
}

void BINetworkManagerClient::SendOutgoingPackets(const BIEngine::GameTimer& gt)
{
   if (m_networkClient.GetState() != BIEngine::NetworkClient::State::Welcomed) {
      return;
   }

   m_networkMessagesManager.SendOutgoingPackets(gt);
}
