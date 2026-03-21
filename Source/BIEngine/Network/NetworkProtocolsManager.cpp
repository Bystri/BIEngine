#include "NetworkProtocolsManager.h"

#include "NetworkProtocol.h"

namespace BIEngine {

#pragma optimize("",off)

void NetworkProtocolsManager::AddProtocolReader(SharedPtr<NetworkProtocolReader> pNetworkProtocolReader)
{
#ifndef _RETIAL
   for (auto& protocol : m_networkProtocolReaders) {
      if (protocol->GetType() == pNetworkProtocolReader->GetType()) {
         Assert(false, "You are trying to add second NetworkProtocolReader with the same type");
         return;
      }
   }
#endif // !_RETIAL

   m_networkProtocolReaders.PushBack(pNetworkProtocolReader);
}

void NetworkProtocolsManager::AddProtocolWriter(SharedPtr<NetworkProtocolWriter> pNetworkProtocolWriter)
{
#ifndef _RETIAL
   for (auto& protocol : m_networkProtocolWriters) {
      uint32_t a = protocol->GetType();
      uint32_t b = pNetworkProtocolWriter->GetType();
      if (protocol->GetType() == pNetworkProtocolWriter->GetType()) {
         Assert(false, "You are trying to add second NetworkProtocolWriter with the same type");
         return;
      }
   }
#endif // !_RETIAL

   m_networkProtocolWriters.PushBack(pNetworkProtocolWriter);
}

void NetworkProtocolsManager::RegisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocolWriters) {
      protocol->RegisterPeer(peerId);
   }

   for (auto& protocol : m_networkProtocolReaders) {
      protocol->RegisterPeer(peerId);
   }
}

void NetworkProtocolsManager::UnregisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocolWriters) {
      protocol->UnregisterPeer(peerId);
   }

   for (auto& protocol : m_networkProtocolReaders) {
      protocol->UnregisterPeer(peerId);
   }
}

void NetworkProtocolsManager::ReceiveMeessage(NetworkProtocolType type, InputMemoryBitStream& stream)
{
   for (auto& protocol : m_networkProtocolReaders) {
      if (protocol->GetType() != type) {
         continue;
      }

      protocol->ReceiveMessage(stream);
      return;
   }

   Assert(false, "Got message for unknown protocol's type");
}

void NetworkProtocolsManager::OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager)
{
   for (auto& protocol : m_networkProtocolWriters) {
      protocol->OnBeforePacketsSend(pNetworkMessagesManager);
   }
}

} // namespace BIEngine
