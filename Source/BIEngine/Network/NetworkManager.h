#pragma once

#include "Peer.h"
#include "Socket.h"
#include "Serialization.h"
#include "NetworkProtocolsManager.h"
#include "../StdLib/SharedPtr.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class NetworkManager {
public:
   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

   virtual ~NetworkManager() {};

   void ProcessIncomingPackets();

   void SendNetworkMessage(const Peer& peer, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream);

protected:
   bool InitInternal(uint16_t port);

   virtual void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress) = 0;

   virtual void HandleConnectionReset(const SocketAddress& fromAddress) {}

private:
   void ReadIncomingPackets();

protected:
   NetworkProtocolsManager m_protocolsManager;

private:
   UdpSocketPtr m_socket;
};

} // namespace BIEngine
