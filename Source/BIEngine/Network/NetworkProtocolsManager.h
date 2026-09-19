#pragma once

#include "NetworkProtocol.h"

namespace BIEngine {

class NetworkProtocolsManager {
public:
   void AddProtocol(SharedPtr<NetworkProtocol> pNetworkProtocol);

   void RegisterPeer(uint32_t peerId);
   void UnregisterPeer(uint32_t peerId);

   void ReceiveMeessage(PeerId peerId, NetworkProtocolType type, InputMemoryBitStream& stream);
   void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager);

private:
   DynamicArray<SharedPtr<NetworkProtocol>> m_networkProtocols;
};

} // namespace BIEngine
