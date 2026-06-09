#include "NetworkProtocolsManager.h"

#include "NetworkProtocol.h"

namespace BIEngine {

#pragma optimize("",off)

void NetworkProtocolsManager::AddProtocolLeader(SharedPtr<NetworkProtocol> pNetworkProtocolLeader)
{
#ifndef _RETIAL
   for (auto& protocol : m_networkProtocolLeaders) {
      if (protocol->GetType() == pNetworkProtocolLeader->GetType()) {
         Assert(false, "You are trying to add second NetworkProtocol leader with the same type");
         return;
      }
   }
#endif // !_RETIAL

   m_networkProtocolLeaders.PushBack(pNetworkProtocolLeader);
}

void NetworkProtocolsManager::AddProtocolFollower(SharedPtr<NetworkProtocol> pNetworkProtocolFollower)
{
#ifndef _RETIAL
   for (auto& protocol : m_networkProtocolFollowers) {
      if (protocol->GetType() == pNetworkProtocolFollower->GetType()) {
         Assert(false, "You are trying to add second NetworkProtocol follower with the same type");
         return;
      }
   }
#endif // !_RETIAL

   m_networkProtocolFollowers.PushBack(pNetworkProtocolFollower);
}

void NetworkProtocolsManager::RegisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocolLeaders) {
      protocol->RegisterPeer(peerId);
   }

   for (auto& protocol : m_networkProtocolFollowers) {
      protocol->RegisterPeer(peerId);
   }
}

void NetworkProtocolsManager::UnregisterPeer(uint32_t peerId)
{
   for (auto& protocol : m_networkProtocolLeaders) {
      protocol->UnregisterPeer(peerId);
   }

   for (auto& protocol : m_networkProtocolFollowers) {
      protocol->UnregisterPeer(peerId);
   }
}

void NetworkProtocolsManager::ReceiveMeessage(NetworkProtocolType type, InputMemoryBitStream& stream)
{
   for (auto& protocol : m_networkProtocolFollowers) {
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
   for (auto& protocol : m_networkProtocolLeaders) {
      protocol->OnBeforePacketsSend(pNetworkMessagesManager);
   }
}

} // namespace BIEngine
