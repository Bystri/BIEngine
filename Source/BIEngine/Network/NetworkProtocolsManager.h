#pragma once

#include "NetworkProtocol.h"

namespace BIEngine {

class NetworkProtocolsManager {
public:
   void AddProtocolLeader(SharedPtr<NetworkProtocol> pNetworkProtocolLeader);
   void AddProtocolFollower(SharedPtr<NetworkProtocol> pNetworkProtocolFollower);

   void RegisterPeer(uint32_t peerId);
   void UnregisterPeer(uint32_t peerId);

   void ReceiveMeessage(NetworkProtocolType type, InputMemoryBitStream& stream);
   void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager);

private:
   DynamicArray<SharedPtr<NetworkProtocol>> m_networkProtocolLeaders;
   DynamicArray<SharedPtr<NetworkProtocol>> m_networkProtocolFollowers;
};

} // namespace BIEngine
