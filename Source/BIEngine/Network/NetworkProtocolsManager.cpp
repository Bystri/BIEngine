#include "NetworkProtocolsManager.h"

#include "NetworkProtocol.h"

namespace BIEngine {

void NetworkProtocolsManager::AddProtocol(SharedPtr<NetworkProtocol> pNetworkProtocol)
{
#ifndef _RETIAL
   for (auto& protocol : m_networkProtocols) {
      if (protocol->GetType() == pNetworkProtocol->GetType()) {
         Assert(false, "You are trying to add second NetworkProtocol with the same type");
         return;
      }
   }
#endif // !_RETIAL

   m_networkProtocols.PushBack(pNetworkProtocol);
}

void NetworkProtocolsManager::RegisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocols) {
      protocol->RegisterPeer(peerId);
   }
}

void NetworkProtocolsManager::UnregisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocols) {
      protocol->UnregisterPeer(peerId);
   }
}

void NetworkProtocolsManager::ReceiveMeessage(PeerId peerId, NetworkProtocolType type, InputMemoryBitStream& stream)
{
   for (auto& protocol : m_networkProtocols) {
      if (protocol->GetType() != type) {
         continue;
      }

      protocol->ReceiveMessage(peerId, stream);
      return;
   }

   Assert(false, "Got message for unknown protocol's type");
}

void NetworkProtocolsManager::OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager)
{
   for (auto& protocol : m_networkProtocols) {
      protocol->OnBeforePacketsSend(pNetworkMessagesManager);
   }
}

} // namespace BIEngine
