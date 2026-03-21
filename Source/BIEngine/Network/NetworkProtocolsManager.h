#pragma once

#include "NetworkProtocol.h"

namespace BIEngine {

class NetworkProtocolsManager {
public:
   void AddProtocolReader(SharedPtr<NetworkProtocolReader> pNetworkProtocolReader);
   void AddProtocolWriter(SharedPtr<NetworkProtocolWriter> pNetworkProtocolWriter);

   void RegisterPeer(uint32_t peerId);
   void UnregisterPeer(uint32_t peerId);

   void ReceiveMeessage(NetworkProtocolType type, InputMemoryBitStream& stream);
   void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager);

private:
   DynamicArray<SharedPtr<NetworkProtocolReader>> m_networkProtocolReaders;
   DynamicArray<SharedPtr<NetworkProtocolWriter>> m_networkProtocolWriters;
};

} // namespace BIEngine
