#pragma once

#include "../BIEngine/StdLib/HashMap.h"
#include "../BIEngine/Network/NetworkServer.h"
#include "../BIEngine/Network/NetworkManager.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"

class BINetworkManagerServer : public BIEngine::NetworkManager {
public:
   bool Init(uint16_t port, int maxClients);

   virtual BIEngine::PeerId GetPeerId() const override { return -1; }

   virtual void Update(const BIEngine::GameTimer& gt) override;

private:
   void OnClientConnected(BIEngine::PeerId clientId);
   void OnClientDisconnected(BIEngine::PeerId clientId);

private:
   BIEngine::NetworkServer m_networkServer;
   BIEngine::DynamicArray<BIEngine::PeerId> m_clients;
   BIEngine::DynamicArray<BIEngine::SharedPtr<ReplicationObjectPlayer>> m_players;
};
