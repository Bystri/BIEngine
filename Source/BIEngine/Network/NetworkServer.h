#pragma once

#include "Peer.h"
#include "Socket.h"
#include "Serialization.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"
#include "../StdLib/UniquePtr.h"

namespace BIEngine {

class NetworkServer {
   using AddressToClientMap = HashMap<SocketAddress, PeerId>;
   using IntToClientMap = HashMap<PeerId, SocketAddress>;
   using IntToQueueMap = HashMap<PeerId, Queue<InputMemoryBitStream>>;

   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kPayloadCC = 'PYLD';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

public:
   bool Init(uint16_t port, std::function<void(PeerId)>&& peerConnectedCb, std::function<void(PeerId)>&& peerDisconnectedCb);
   bool IsClientConnected(PeerId clientId) const;
   int GetConnectedClientsNum() const { return m_addressToClientMap.Size(); }

   void Update();

   UniquePtr<InputMemoryBitStream> ReceivePacket(PeerId clientId);
   bool SendPacket(PeerId clientId, const OutputMemoryBitStream& outputStream);

private:
   void ReadIncomingPackets();

   void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void ProcessPacket(PeerId clientId, InputMemoryBitStream& inputStream);
   void SendPacketInternal(const OutputMemoryBitStream& outputStream, const SocketAddress& fromAddress);

   void SendWelcomePacket(PeerId clientId);

   void HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleConnectionReset(const SocketAddress& fromAddress);

private:
   UdpSocketPtr m_socket;
   AddressToClientMap m_addressToClientMap;
   IntToClientMap m_clientIdxToAddressMap;
   IntToQueueMap m_clintIdxToPayloadPacketQueueMap;
   DynamicArray<PeerId> m_clientIds;
   PeerId m_nextClientId = 1u;

   std::function<void(PeerId)> m_peerConnectedCb;
   std::function<void(PeerId)> m_peerDisconnectedCb;
};

} // namespace BIEngine
