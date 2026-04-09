#pragma once

#include <queue>

#include "Peer.h"
#include "Socket.h"
#include "Serialization.h"
#include "NetworkMessagesManager.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class NetworkManager {
public:
   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kEstablishedCC = 'ESTB';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

   virtual ~NetworkManager() {};

   virtual PeerId GetPeerId() const = 0;

   virtual void Update(const GameTimer& gt) = 0;
   void ProcessIncomingPackets();
   void SendOutgoingPackets(const BIEngine::GameTimer& gt);

   void SendNetworkMessage(PeerId peedId, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream);

   float GetRttForPeer(PeerId peerId) const { return m_networkMessagesManager.GetRttForPeer(peerId); }

#ifndef _RETAIL
   void DrawDbgDiagnostics();
#endif

protected:
   bool InitInternal(uint16_t port);

   virtual void HandleConnectionReset(const SocketAddress& fromAddress) {}

   void SendMessagesFromQueue(const GameTimer& gt);

protected:
   NetworkMessagesManager m_networkMessagesManager;
   Deque<int> m_processedMessagesIds;
};

} // namespace BIEngine
