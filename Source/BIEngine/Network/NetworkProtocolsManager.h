#pragma once

#include "NetworkProtocol.h"

namespace BIEngine {

class NetworkProtocolsManager {
public:
   void AddProtocolReader(SharedPtr<NetworkProtocolReader> pNetworkProtocolReader);
   void AddProtocolWriter(SharedPtr<NetworkProtocolWriter> pNetworkProtocolWriter);

   void RegisterPeer(PeerPtr pPeer);
   void UnregisterPeer(PeerPtr pPeer);

   void ReceiveMeessage(NetworkProtocolType type, InputMemoryBitStream& stream);
   void OnBeforePacketsSend(NetworkManager* pNetworkManager);

private:
   DynamicArray<SharedPtr<NetworkProtocolReader>> m_networkProtocolReaders;
   DynamicArray<SharedPtr<NetworkProtocolWriter>> m_networkProtocolWriters;
};

} // namespace BIEngine
