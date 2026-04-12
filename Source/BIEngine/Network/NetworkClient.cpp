#include "NetworkClient.h"

namespace BIEngine {

static constexpr float TIME_BETWEEN_HELLOS = 2.f;

bool NetworkClient::Init(const BIEngine::SocketAddress& serverAddress, std::function<void()>&& serverWelcomedClientCb, std::function<void()>&& serverDisconnectedCb)
{
   m_socket = SocketUtil::CreateUdpSocket(SocketAddressFamily::INET);
   const SocketAddress ownAddress(INADDR_ANY, 0);
   m_socket->Bind(ownAddress);

   if (m_socket == nullptr) {
      return false;
   }

   if (m_socket->SetNonBlockingMode(true) != NO_ERROR) {
      return false;
   }

   m_serverWelcomedClientCb = serverWelcomedClientCb;
   m_serverDisconnectedCb = serverDisconnectedCb;

   m_serverAddress = serverAddress;

   m_state = State::SayingHello;
   m_timeOfLastHello = 0.0f;

   Logger::WriteMsgLog("NetworkClient inited");

   return true;
}

void NetworkClient::Update(float totalTime)
{
   ReadIncomingPackets();

   switch (m_state) {
      case State::SayingHello:
         UpdateSayingHello(totalTime);
         break;
      case State::Welcomed:
         // Do nothing
         break;
      case State::Disconnected:
         // Do nothing
         break;
      default:
         Assert(false, "You are trying to call NetworkClient::Update() for uninitialized NetworkClient!");
         break;
   }
}

UniquePtr<InputMemoryBitStream> NetworkClient::ReceivePacket()
{
   if (m_payloadPacketsToProcess.Empty()) {
      return nullptr;
   }

   UniquePtr<InputMemoryBitStream> ret = MakeUnique<InputMemoryBitStream>(std::move(m_payloadPacketsToProcess.Front()));
   m_payloadPacketsToProcess.Pop();
   return ret;
}

bool NetworkClient::SendPacket(const OutputMemoryBitStream& outputStream)
{
   // CAN HAS m_clientIdxToAddressMap AFTER WELCOMED
   if (m_state != State::Welcomed) {
      return false;
   }

   OutputMemoryBitStream packet;
   Serialize(packet, kPayloadCC);
   packet.WriteBits(outputStream.GetBufferPtr().Get(), outputStream.GetBitLength());

   SendPacketInternal(packet);

   return true;
}

// ALL THE SAME
void NetworkClient::SendPacketInternal(const OutputMemoryBitStream& outputStream)
{
   m_socket->SendTo(outputStream.GetBufferPtr().Get(), outputStream.GetByteLength(), m_serverAddress);
}

// ALL THE SAME
void NetworkClient::ReadIncomingPackets()
{
   SocketAddress fromAddress;

   int receivedPackedCount = 0;

   while (receivedPackedCount < MAX_PACKETS_PER_FRAME_COUNT) {
      constexpr int packetSize = 1500;
      SharedPtr<char> segment(static_cast<char*>(std::malloc(packetSize)), std::free);
      InputMemoryBitStream inputStream(segment, packetSize * 8);

      int readByteCount = m_socket->ReceiveFrom(segment.Get(), packetSize, fromAddress);
      if (readByteCount == 0) {
         // nothing to read
         break;
      } else if (readByteCount == -WSAECONNRESET) {
         // port closed on other end, so DC this person immediately
         if (m_state != State::Welcomed) {
            continue;
         }
         HandleConnectionReset();
      } else if (readByteCount > 0) {
         inputStream.ResetToCapacity(readByteCount);
         ++receivedPackedCount;
         ProcessPacket(inputStream, fromAddress);
      } else {
         // uhoh, error? exit or just keep going?
      }
   }
}

void NetworkClient::ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);
   switch (packetType) {
      case kWelcomeCC:
         HandleWelcomePacket(inputStream);
         break;
      case kPayloadCC:
         if (m_state == State::Welcomed) {
            m_payloadPacketsToProcess.Push(inputStream);
         }
         break;
   }
}

void NetworkClient::UpdateSayingHello(float totalTime)
{
   if (totalTime > m_timeOfLastHello + TIME_BETWEEN_HELLOS) {
      SendHelloPacket();
      m_timeOfLastHello = totalTime;
   }
}

void NetworkClient::SendHelloPacket()
{
   BIEngine::OutputMemoryBitStream helloPacket;

   BIEngine::Serialize(helloPacket, kHelloCC);

   BIEngine::Logger::WriteMsgLog("Send hello packet to server");
   SendPacketInternal(helloPacket);
}

void NetworkClient::HandleWelcomePacket(BIEngine::InputMemoryBitStream& inputStream)
{
   if (m_state == State::SayingHello) {
      BIEngine::Deserialize(inputStream, m_clientId);
      m_state = State::Welcomed;
      m_serverWelcomedClientCb();

      BIEngine::Logger::WriteMsgLog("Client was welcomed on Server as player %d", m_clientId);
   }
}

void NetworkClient::HandleConnectionReset()
{
   m_serverDisconnectedCb();
   m_state = State::Disconnected;
}

} // namespace BIEngine
