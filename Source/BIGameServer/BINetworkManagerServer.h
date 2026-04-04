#pragma once

#include "../BIEngine/StdLib/HashMap.h"
#include "../BIEngine/Network/NetworkServer.h"
#include "../BIEngine/Network/NetworkManager.h"

class BINetworkManagerServer : public BIEngine::NetworkManager {
public:
   bool Init(uint16_t port);

   virtual uint32_t GetPeerId() const override { return -1; }

   virtual void Update(const BIEngine::GameTimer& gt) override;

private:
   BIEngine::NetworkServer m_networkServer;
};
