#include "BINetworkManagerServer.h"

#include "../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIEngine/Network/RpcProtocol.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

#include "BIGSEventListener.h"

bool BINetworkManagerServer::Init(uint16_t port, int maxClients)
{
   m_networkMessagesManager.AddProtocolWriter(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolWriter>());
   m_networkMessagesManager.AddProtocolWriter(BIEngine::MakeShared<BIEngine::RpcProtocolWriter>());
   m_networkMessagesManager.AddProtocolReader(BIEngine::MakeShared<EventProtocolReader>());

   m_clients = BIEngine::DynamicArray<BIEngine::PeerId>(maxClients, BIEngine::INVALID_PEER_ID);

   return m_networkServer.Init(port, [this](BIEngine::PeerId id) { OnClientConnected(id); }, [this](BIEngine::PeerId id) { OnClientDisconnected(id); });
}

void BINetworkManagerServer::Update(const BIEngine::GameTimer& gt)
{
   m_networkServer.Update();

   for (int i = 0; i < m_clients.Size(); ++i) {
      if (m_clients[i] == BIEngine::INVALID_PEER_ID) {
         continue;
      }

      while (true) {
         // We can have client with different id by this idx
         BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pPacketData = m_networkServer.ReceivePacket(m_clients[i]);
         if (pPacketData == nullptr) {
            break;
         }

         m_networkMessagesManager.ProcessPacket(m_clients[i], *pPacketData, gt);
      }
   }
}

void BINetworkManagerServer::OnClientConnected(BIEngine::PeerId clientId)
{
   BIEngine::Assert(clientId != BIEngine::INVALID_PEER_ID, "Got invalid clientId");

   for (int i = 0; i < m_clients.Size(); ++i) {
      if (m_clients[i] != BIEngine::INVALID_PEER_ID) {
         continue;
      }

      m_clients[i] = clientId;
      m_networkMessagesManager.RegisterPeer(clientId, BIEngine::g_pApp->GetGameTimer(), std::bind(&BIEngine::NetworkServer::SendPacket, m_networkServer, clientId, std::placeholders::_1));

      BIEngine::SharedPtr<EvtData_NetPeer_Connected> pEvent = BIEngine::MakeShared<EvtData_NetPeer_Connected>(clientId);
      BIEngine::EventManager::Get()->TriggerEvent(pEvent);
      return;
   }
}

void BINetworkManagerServer::OnClientDisconnected(BIEngine::PeerId clientId)
{
   for (int i = 0; i < m_clients.Size(); ++i) {
      if (m_clients[i] != clientId) {
         continue;
      }

      m_clients[i] = BIEngine::INVALID_PEER_ID;
      m_networkMessagesManager.UnregisterPeer(clientId);

      BIEngine::SharedPtr<EvtData_NetPeer_Disonnected> pEvent = BIEngine::MakeShared<EvtData_NetPeer_Disonnected>(clientId);
      BIEngine::EventManager::Get()->TriggerEvent(pEvent);

      return;
   }
}
