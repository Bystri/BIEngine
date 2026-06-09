#include "BINetworkManagerClient.h"

#include "../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIEngine/Network/RpcProtocol.h"
#include "../BIGame/Network/BINetworkRPCs.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

bool BINetworkManagerClient::Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name)
{
   m_networkMessagesManager.AddProtocolFollower(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolFollower>());
   m_networkMessagesManager.AddProtocolLeader(BIEngine::MakeShared<EventProtocolLeader>());
   m_networkMessagesManager.AddProtocolFollower(BIEngine::MakeShared<BIEngine::RpcProtocolFollower>());
   RpcInit();

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