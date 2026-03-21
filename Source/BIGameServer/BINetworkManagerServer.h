#pragma once

#include "../BIEngine/StdLib/HashMap.h"
#include "../BIEngine/Network/NetworkServer.h"
#include "../BIEngine/Network/NetworkManager.h"

class BINetworkManagerServer : public BIEngine::NetworkManager {
public:
   bool Init(uint16_t port);

   virtual void Update(const BIEngine::GameTimer& gt) override;
   void SendOutgoingPackets(const BIEngine::GameTimer& gt);

private:
   BIEngine::NetworkServer m_networkServer;
   uint32_t m_nextClientId = 0u;
};
