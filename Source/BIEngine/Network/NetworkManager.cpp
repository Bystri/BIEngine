#include "NetworkManager.h"

#include "../Utilities/Logger.h"

namespace BIEngine {

bool NetworkManager::InitInternal(uint16_t port)
{
   m_socket = SocketUtil::CreateUdpSocket(SocketAddressFamily::INET);
   const SocketAddress ownAddress(INADDR_ANY, port);
   m_socket->Bind(ownAddress);

   Logger::WriteMsgLog("Initializing NetworkManager at port %d", port);

   if (m_socket == nullptr) {
      return false;
   }

   if (m_socket->SetNonBlockingMode(true) != NO_ERROR) {
      return false;
   }

   return true;
}

void NetworkManager::ProcessIncomingPackets()
{
   ReadIncomingPackets();
}

void NetworkManager::ReadIncomingPackets()
{
   constexpr int packetSize = 1500;
   std::shared_ptr<char> segment(static_cast<char*>(std::malloc(packetSize)), std::free);
   InputMemoryBitStream inputStream(segment, packetSize * 8);
   SocketAddress fromAddress;

   int receivedPackedCount = 0;

   while (receivedPackedCount < MAX_PACKETS_PER_FRAME_COUNT) {
      int readByteCount = m_socket->ReceiveFrom(segment.get(), packetSize, fromAddress);
      if (readByteCount == 0) {
         // nothing to read
         break;
      } else if (readByteCount == -WSAECONNRESET) {
         // port closed on other end, so DC this person immediately
         HandleConnectionReset(fromAddress);
      } else if (readByteCount > 0) {
         inputStream.ResetToCapacity(readByteCount);
         ++receivedPackedCount;

         ProcessPacket(inputStream, fromAddress);
      } else {
         // uhoh, error? exit or just keep going?
      }
   }
}

void NetworkManager::SendPacket(const OutputMemoryBitStream& outputStream, const SocketAddress& fromAddress)
{
   BIEngine::Logger::WriteMsgLog("SendPacket with byte length %d", outputStream.GetByteLength());
   m_socket->SendTo(outputStream.GetBufferPtr().get(), outputStream.GetByteLength(), fromAddress);

   InputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());
   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);
   BIEngine::Logger::WriteMsgLog("Packet type %zu", packetType);
}

} // namespace BIEngine
