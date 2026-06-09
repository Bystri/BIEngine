#pragma once

#include "Peer.h"
#include "Serialization.h"

namespace BIEngine {

using NetworkProtocolType = uint32_t;

class NetworkManager;
class NetworkMessagesManager;

class NetworkProtocol {
    friend class NetworkProtocolsManager;

public:
   static const NetworkProtocolType sk_ProtocolType;

   virtual ~NetworkProtocol() = default;

protected:
   virtual void RegisterPeer(PeerId peerId) {}

   virtual void UnregisterPeer(PeerId peerId) {}

   virtual void ReceiveMessage(InputMemoryBitStream& stream) {}
   virtual void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager) {}

   virtual const NetworkProtocolType& GetType() const { return sk_ProtocolType; }
};

} // namespace BIEngine
