#include "NetworkServer.h"

namespace BIEngine {

bool NetworkServer::Init(uint16_t port)
{
   m_socket = SocketUtil::CreateUdpSocket(SocketAddressFamily::INET);
   const SocketAddress ownAddress(INADDR_ANY, port);
   m_socket->Bind(ownAddress);

   if (m_socket == nullptr) {
      return false;
   }

   if (m_socket->SetNonBlockingMode(true) != NO_ERROR) {
      return false;
   }

   Logger::WriteMsgLog("NetworkServer inited");

   return true;
}

uint32_t NetworkServer::GetClientId(int clientIdx)
{
   return m_clientIds[clientIdx];
}

bool NetworkServer::IsClientConnected(uint32_t clientId) const
{
   return m_clientIdxToAddressMap.Find(clientId) != m_clientIdxToAddressMap.End();
}

void NetworkServer::Update()
{
   ReadIncomingPackets();
}

UniquePtr<InputMemoryBitStream> NetworkServer::ReceivePacket(uint32_t clientId)
{
   if (!IsClientConnected(clientId)) {
      return nullptr;
   }

   auto& payloadPacketQueue = m_clintIdxToPayloadPacketQueueMap[clientId];
   if (payloadPacketQueue.Empty()) {
      return nullptr;
   }

   UniquePtr<InputMemoryBitStream> ret = MakeUnique<InputMemoryBitStream>(std::move(payloadPacketQueue.Front()));
   payloadPacketQueue.Pop();
   return ret;
}

bool NetworkServer::SendPacket(uint32_t clientId, const OutputMemoryBitStream& outputStream)
{
   // SEE NetworkClient
   auto itr = m_clientIdxToAddressMap.Find(clientId);
   if (itr == m_clientIdxToAddressMap.End()) {
      return false;
   }

   OutputMemoryBitStream packet;
   Serialize(packet, kPayloadCC);
   packet.WriteBits(outputStream.GetBufferPtr().Get(), outputStream.GetBitLength());
   SendPacketInternal(packet, itr->second);

   return true;
}

// ALL THE SAME
void NetworkServer::SendPacketInternal(const OutputMemoryBitStream& outputStream, const SocketAddress& fromAddress)
{
   m_socket->SendTo(outputStream.GetBufferPtr().Get(), outputStream.GetByteLength(), fromAddress);
}

// ALL THE SAME
void NetworkServer::ReadIncomingPackets()
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

void NetworkServer::ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // try to get the client proxy for this address
   // pass this to the client proxy to process
   auto it = m_addressToClientMap.Find(fromAddress);
   if (it == m_addressToClientMap.End()) {
      // didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
      HandlePacketFromNewClient(inputStream, fromAddress);
   } else {
      ProcessPacket((*it).second, inputStream);
   }
}

void NetworkServer::ProcessPacket(uint32_t clientId, InputMemoryBitStream& inputStream)
{
   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);
   switch (packetType) {
      case kHelloCC:
         // need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
         // otherwise something weird is going on...
         SendWelcomePacket(clientId);
         break;
      case kPayloadCC:
         m_clintIdxToPayloadPacketQueueMap[clientId].Push(inputStream);
         break;
      default:
         BIEngine::Logger::WriteErrorLog("Unknown packet type received");
         break;
   }
}

void NetworkServer::SendWelcomePacket(uint32_t clientId)
{
   OutputMemoryBitStream welcomePacket;

   Serialize(welcomePacket, kWelcomeCC);
   Serialize(welcomePacket, clientId);

   Logger::WriteMsgLog("Server Welcoming, new client with id %d", clientId);

   SendPacketInternal(welcomePacket, m_clientIdxToAddressMap[clientId]);
}

void NetworkServer::HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // read the beginning- is it a hello?
   uint32_t packetType;
   Deserialize(inputStream, packetType);

   if (packetType == kHelloCC) {
      m_addressToClientMap[fromAddress] = m_nextClientId;
      m_clientIdxToAddressMap.Insert(m_nextClientId, fromAddress);
      m_clientIds.PushBack(m_nextClientId);

      // and welcome the client...
      SendWelcomePacket(m_nextClientId);

      ++m_nextClientId;

   } else {
      // bad incoming packet from unknown client- we're under attack!!
      Logger::WriteErrorLog("Bad incoming packet from unknown client");
   }
}

void NetworkServer::HandleConnectionReset(const SocketAddress& fromAddress)
{
   const uint32_t disconnectedClient = m_addressToClientMap[fromAddress];

   m_addressToClientMap.Erase(fromAddress);
   m_clientIdxToAddressMap.Erase(disconnectedClient);
   m_clintIdxToPayloadPacketQueueMap.Erase(disconnectedClient);
   m_clientIds.Erase(Find(m_clientIds.Begin(), m_clientIds.End(), disconnectedClient));
}

} // namespace BIEngine
