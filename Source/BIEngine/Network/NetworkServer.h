#pragma once

#include "Socket.h"
#include "Serialization.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"
#include "../StdLib/UniquePtr.h"

namespace BIEngine {

class NetworkServer {
   using AddressToClientMap = HashMap<SocketAddress, uint32_t>;
   using IntToClientMap = HashMap<uint32_t, SocketAddress>;
   using IntToQueueMap = HashMap<uint32_t, Queue<InputMemoryBitStream>>;

   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kPayloadCC = 'PYLD';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

public:
   bool Init(uint16_t port);

   uint32_t GetClientId(int clientIdx);
   bool IsClientConnected(uint32_t clientId) const;

   int GetConnectedClients() const { return m_addressToClientMap.Size(); }

   void Update();

   UniquePtr<InputMemoryBitStream> ReceivePacket(uint32_t clientId);
   bool SendPacket(uint32_t clientId, const OutputMemoryBitStream& outputStream);

private:
   void ReadIncomingPackets();

   void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void ProcessPacket(uint32_t clientId, InputMemoryBitStream& inputStream);
   void SendPacketInternal(const OutputMemoryBitStream& outputStream, const SocketAddress& fromAddress);

   void SendWelcomePacket(uint32_t clientId);

   void HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleWelcomePacket(InputMemoryBitStream& inputStream);
   void HandleConnectionReset(const SocketAddress& fromAddress);

private:
   UdpSocketPtr m_socket;
   AddressToClientMap m_addressToClientMap;
   IntToClientMap m_clientIdxToAddressMap;
   IntToQueueMap m_clintIdxToPayloadPacketQueueMap;
   DynamicArray<int32_t> m_clientIds;
   uint32_t m_nextClientId = 0u;
};

} // namespace BIEngine
