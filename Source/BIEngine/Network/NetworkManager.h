#pragma once

#include <queue>

#include "Peer.h"
#include "Socket.h"
#include "Serialization.h"
#include "NetworkProtocolsManager.h"
#include "DeliveryNotificationManager.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class NetworkManager {
   friend class NetworkTransmissionData;

protected:
   class MessageToSend {
   public:
      explicit MessageToSend(uint32_t id)
         : m_id(id)
      {
      }

      uint32_t GetId() const { return m_id; }

      OutputMemoryBitStream& GetBuffer() { return m_buffer; }

      const OutputMemoryBitStream& GetBuffer() const { return m_buffer; }

      void Write(OutputMemoryBitStream& stream) const
      {
         Serialize(stream, m_id);
         Serialize(stream, m_buffer.GetByteLength());
         stream.WriteBytes(m_buffer.GetBufferPtr().Get(), m_buffer.GetByteLength());
      }

   private:
      uint32_t m_id;
      OutputMemoryBitStream m_buffer;
   };

   class MessageToRead {
   public:
      bool operator<(const MessageToRead& r) const
      {
         return GetId() < r.GetId();
      }

      MessageToRead()
         : m_id(0), m_buffer(nullptr, 0)
      {
      }

      uint32_t GetId() const { return m_id; }

      InputMemoryBitStream& GetBuffer() { return m_buffer; }

      const InputMemoryBitStream& GetBuffer() const { return m_buffer; }

      void Read(InputMemoryBitStream& stream)
      {
         Deserialize(stream, m_id);

         uint32_t bufferByteSize;
         Deserialize(stream, bufferByteSize);

         SharedPtr<char> segment(static_cast<char*>(std::malloc(bufferByteSize)), std::free);
         m_buffer = InputMemoryBitStream(segment, bufferByteSize * 8);

         stream.ReadBytes(m_buffer.GetBufferPtr().Get(), bufferByteSize);
      }

   private:
      uint32_t m_id;
      InputMemoryBitStream m_buffer;
   };

   struct PeerInfo {
      PeerInfo(const PeerPtr& peer)
         : pPeer(peer)
      {
      }

      PeerPtr pPeer;

      uint32_t messageId = 0u;
      uint32_t expectedMessageId = 1u;
      Queue<MessageToSend> messageQueueToSend;
      DynamicArray<MessageToRead> messageQueueToRead;

      DeliveryNotificationManager deliveryNotificationManager;
   };

   class NetworkTransmissionData : public TransmissionData {
   public:
      NetworkTransmissionData(PeerPtr pPeer, const MessageToSend& msg, NetworkManager* pNetworkServer)
         : m_pPeer(pPeer), m_msg(msg), m_pNetworkServer(pNetworkServer)
      {
      }

      virtual void HandleDeliverySuccess(DeliveryNotificationManager* pDeliveryNotificationManager) const override
      {
      }

      virtual void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const override
      {
         m_pNetworkServer->ResendNetworkMessage(*m_pPeer, m_msg);
      }

   private:
      PeerPtr m_pPeer;
      MessageToSend m_msg;
      NetworkManager* m_pNetworkServer;
   };

public:
   static const uint32_t kHelloCC = 'HELO';
   static const uint32_t kWelcomeCC = 'WLCM';
   static const uint32_t kEstablishedCC = 'ESTB';
   static constexpr int MAX_PACKETS_PER_FRAME_COUNT = 10;

   virtual ~NetworkManager() {};

   void ProcessIncomingPackets();

   void SendNetworkMessage(const Peer& peer, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream);

#ifndef _RETAIL
   void DrawDbgDiagnostics();
#endif

protected:
   bool InitInternal(uint16_t port);

   virtual void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress) = 0;

   virtual void HandleConnectionReset(const SocketAddress& fromAddress) {}

   void SendMessagesFromQueue(const GameTimer& gt);

private:
   void ResendNetworkMessage(const Peer& peer, const MessageToSend& msg)
   {
      auto peerInfoPtr = m_peerInfoMap.Find(peer.GetId());
      if (peerInfoPtr == m_peerInfoMap.End()) {
         Assert(false, "You are trying to send message to an unconnected peer");
         return;
      }

      PeerInfo& peerInfo = peerInfoPtr->second;

      Logger::WriteMsgLog("Resend msg with id %u", msg.GetId());
      peerInfo.messageQueueToSend.Push(msg);
   }

   void ReadIncomingPackets();

protected:
   NetworkProtocolsManager m_protocolsManager;

   HashMap<uint32_t, PeerInfo> m_peerInfoMap;

   Deque<int> m_processedMessagesIds;

private:
   UdpSocketPtr m_socket;
};

} // namespace BIEngine
