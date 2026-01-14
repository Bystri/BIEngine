#pragma once

#include "Peer.h"
#include "Serialization.h"

namespace BIEngine {

using NetworkProtocolType = uint32_t;

class NetworkManager;

class NetworkProtocol {
public:
   static const NetworkProtocolType sk_ProtocolType;

   virtual ~NetworkProtocol() = default;

protected:
   virtual void RegisterPeer(PeerPtr pPeer) {}

   virtual void UnregisterPeer(PeerPtr pPeer) {}

   virtual const NetworkProtocolType& GetType() const { return sk_ProtocolType; }
};

class NetworkProtocolReader : public NetworkProtocol {
   friend class NetworkProtocolsManager;

protected:
   virtual void ReceiveMessage(InputMemoryBitStream& stream) = 0;
};

class NetworkProtocolWriter : public NetworkProtocol {
   friend class NetworkProtocolsManager;

protected:
   virtual void OnBeforePacketsSend(NetworkManager* pNetworkManager) = 0;
};

} // namespace BIEngine
