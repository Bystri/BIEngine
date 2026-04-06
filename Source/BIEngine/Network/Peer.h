#pragma once

#include "Socket.h"

namespace BIEngine {

using PeerId = uint32_t;

class Peer {
public:
   Peer(PeerId id, SocketAddress socketAddress)
      : m_id(id), m_socketAddress(std::move(socketAddress))
   {
   }

   PeerId GetId() const { return m_id; }

   const SocketAddress& GetSocketAddress() const { return m_socketAddress; }

private:
   PeerId m_id;
   SocketAddress m_socketAddress;
};

using PeerPtr = SharedPtr<Peer>;

constexpr PeerId INVALID_PEER_ID = static_cast<PeerId>(-1);

} // namespace BIEngine
