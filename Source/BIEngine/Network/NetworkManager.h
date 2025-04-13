#pragma once

#include <queue>
#include <list>

#include "Socket.h"
#include "Serialization.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class Peer {
public:
   Peer(int id, SocketAddress socketAddress)
      : m_id(id), m_socketAddress(std::move(socketAddress))
   {
   }

   int GetId() const { return m_id; }

   const SocketAddress& GetSocketAddress() const { return m_socketAddress; }

private:
   int m_id;
   SocketAddress m_socketAddress;
};

using PeerPtr = std::shared_ptr<Peer>;

class NetworkManager {
public:
   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kStateCC = 'STAT';
   static const uint32_t kEventCC = 'EVNT';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

   virtual ~NetworkManager() {};

   void ProcessIncomingPackets();

   void SendPacket(const OutputMemoryBitStream& outputStream, const SocketAddress& fromAddress);

protected:
   bool InitInternal(uint16_t port);

   virtual void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress) = 0;

   virtual void HandleConnectionReset(const SocketAddress& fromAddress) {}

private:
   void ReadIncomingPackets();

private:
   UdpSocketPtr m_socket;
};

} // namespace BIEngine
