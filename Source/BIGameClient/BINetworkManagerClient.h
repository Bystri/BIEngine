#pragma once

#include "../BIEngine/StdLib/DynamicArray.h"
#include "../BIEngine/Network/NetworkManager.h"
#include "../BIEngine/Network/NetworkClient.h"
#include "../BIEngine/EventManager/EventManager.h"

class BINetworkManagerClient : public BIEngine::NetworkManager {
public:
   bool Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name);

   BIEngine::PeerId GetPeerId() const override { return m_networkClient.GetClientId(); }

   virtual void Update(const BIEngine::GameTimer& gt) override;

private:
   BIEngine::NetworkClient m_networkClient;
   BIEngine::PeerPtr m_pServerPeer;

   BIEngine::String m_name;
   int m_playerId;
};
