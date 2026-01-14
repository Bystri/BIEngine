#pragma once

#include "Socket.h"

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

using PeerPtr = SharedPtr<Peer>;

} // namespace BIEngine
