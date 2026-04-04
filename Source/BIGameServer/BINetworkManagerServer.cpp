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
      const uint32_t peerId = m_networkServer.GetClientId(i);

      auto infoItr = m_peerInfoMap.Find(peerId);

      // TODO: Replace
      if (infoItr == m_peerInfoMap.End()) {
         BIEngine::SocketAddress address;
         auto peer = BIEngine::MakeShared<BIEngine::Peer>(peerId, address);
         m_peerInfoMap.Emplace(peerId, peer);
         m_networkMessagesManager.RegisterPeer(peerId, gt, std::bind(&BIEngine::NetworkServer::SendPacket, m_networkServer, peerId, std::placeholders::_1));
         BIEngine::ObjectReplicationCreate(ReplicationObjectPlayer::sk_ClassType);
      }

      while (true) {
         // We can have client with different id by this idx
         BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pPacketData = m_networkServer.ReceivePacket(peerId);
         if (pPacketData == nullptr) {
            break;
         }

         m_networkMessagesManager.ProcessPacket(peerId, *pPacketData, gt);
      }
   }
}
