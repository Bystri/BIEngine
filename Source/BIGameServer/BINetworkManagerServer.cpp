#include "BINetworkManagerServer.h"

#include "../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

#include "BIGSEventListener.h"

bool BINetworkManagerServer::Init(uint16_t port)
{
   m_networkMessagesManager.AddProtocolWriter(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolWriter>());
   m_networkMessagesManager.AddProtocolReader(BIEngine::MakeShared<EventProtocolReader>());
   return m_networkServer.Init(port);
}

void BINetworkManagerServer::Update(const BIEngine::GameTimer& gt)
{
   m_networkServer.Update();

   for (int i = 0; i < m_networkServer.GetConnectedClients(); ++i) {
      while (true) {
         auto infoItr = m_peerInfoMap.Find(i);

         // TODO: Replace
         if (infoItr == m_peerInfoMap.End()) {
            BIEngine::SocketAddress address;
            auto peer = BIEngine::MakeShared<BIEngine::Peer>(i, address);
            m_peerInfoMap.Emplace(i, peer);
            m_networkMessagesManager.RegisterPeer(i,
                std::bind(&BIEngine::NetworkServer::SendPacket, m_networkServer, i, std::placeholders::_1));
            BIEngine::ObjectReplicationCreate(ReplicationObjectPlayer::sk_ClassType);
         }

         // We can have client with different id by this idx
         BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pPacketData = m_networkServer.ReceivePacket(i);
         if (pPacketData == nullptr) {
            break;
         }

         m_networkMessagesManager.ProcessPacket(i, *pPacketData);
      }
   }
}

void BINetworkManagerServer::SendOutgoingPackets(const BIEngine::GameTimer& gt)
{
   m_networkMessagesManager.SendOutgoingPackets(gt);
}
