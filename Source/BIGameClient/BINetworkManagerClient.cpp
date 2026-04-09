#include "BINetworkManagerClient.h"

#include "../../../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

static constexpr float TIME_BETWEEN_HELLOS = 2.f;

static constexpr float TIME_BETWEEN_EVENT_PACKETS = 0.033f;

bool BINetworkManagerClient::Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name)
{
   m_networkMessagesManager.AddProtocolReader(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolReader>());
   m_networkMessagesManager.AddProtocolWriter(BIEngine::MakeShared<EventProtocolWriter>());
   m_name = name;

   return m_networkClient.Init(serverAddress, [this]() { OnWelcomed(); }, [this]() { OnDisconnected(); });
}

void BINetworkManagerClient::Update(const BIEngine::GameTimer& gt)
{
   m_networkClient.Update(gt.TotalTime());

   while (true) {
      BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pPacketData = m_networkClient.ReceivePacket();
      if (pPacketData == nullptr) {
         break;
      }

      m_networkMessagesManager.ProcessPacket(m_serverPeer, *pPacketData, gt);
   }
}

void BINetworkManagerClient::OnWelcomed()
{
   m_networkMessagesManager.RegisterPeer(
      m_serverPeer,
      BIEngine::g_pApp->GetGameTimer(),
      std::bind(&BIEngine::NetworkClient::SendPacket, m_networkClient, std::placeholders::_1));
}

void BINetworkManagerClient::OnDisconnected()
{
   m_networkMessagesManager.UnregisterPeer(m_serverPeer);
}