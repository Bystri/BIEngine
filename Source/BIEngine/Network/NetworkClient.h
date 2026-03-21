#pragma once

#include "Socket.h"
#include "Serialization.h"
#include "../StdLib/Queue.h"
#include "../StdLib/UniquePtr.h"

namespace BIEngine {

class NetworkClient {
   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kPayloadCC = 'PYLD';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

public:
   enum class State {
      Uninitialized,
      SayingHello,
      Welcomed,
      Disconnected
   };

public:
   bool Init(const SocketAddress& serverAddress);

   int32_t GetClientId() const { return m_clientId; }

   State GetState() const { return m_state; }

   void Update(float totalTime);

   UniquePtr<InputMemoryBitStream> ReceivePacket();
   bool SendPacket(const OutputMemoryBitStream& outputStream);

private:
   void ReadIncomingPackets();
   void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void SendPacketInternal(const OutputMemoryBitStream& outputStream);

   void UpdateSayingHello(float totalTime);
   void SendHelloPacket();

   void HandleWelcomePacket(InputMemoryBitStream& inputStream);
   void HandleConnectionReset();

private:
   UdpSocketPtr m_socket;
   SocketAddress m_serverAddress;
   Queue<InputMemoryBitStream> m_payloadPacketsToProcess;

   State m_state;
   int32_t m_clientId;

   float m_timeOfLastHello;
};

} // namespace BIEngine
