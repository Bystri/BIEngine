#pragma once

#include "Serialization.h"
#include "DeliveryNotificationManager.h"
#include "NetworkProtocolsManager.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"

namespace BIEngine {

class NetworkMessagesManager {
private:
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

   class NetworkTransmissionData : public TransmissionData {
   public:
      NetworkTransmissionData(uint32_t peerId, const MessageToSend& msg, NetworkMessagesManager* pMsgMessager)
         : m_peerId(peerId), m_msg(msg), m_pMsgMessager(pMsgMessager)
      {
      }

      virtual void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const override
      {
         m_pMsgMessager->ResendNetworkMessage(m_peerId, m_msg);
      }

   private:
      uint32_t m_peerId;
      MessageToSend m_msg;
      NetworkMessagesManager* m_pMsgMessager;
   };

   struct PeerInfo {
      uint32_t peerId;

      uint32_t messageId = 0u;
      uint32_t expectedMessageId = 0u;
      Queue<MessageToSend> messageQueueToSend;
      DynamicArray<MessageToRead> messageQueueToRead;
      DeliveryNotificationManager deliveryNotificationManager;

      std::function<void(const OutputMemoryBitStream&)> sendFunc;
   };

public:
    NetworkMessagesManager()
    {
    }

    void AddProtocolReader(SharedPtr<NetworkProtocolReader> pNetworkProtocolReader)
    {
       m_protocolsManager.AddProtocolReader(pNetworkProtocolReader);
    }

    void AddProtocolWriter(SharedPtr<NetworkProtocolWriter> pNetworkProtocolWriter)
    {
       m_protocolsManager.AddProtocolWriter(pNetworkProtocolWriter);
    }

   void RegisterPeer(uint32_t peerId, const std::function<void(const OutputMemoryBitStream&)>& sendFunc);
   void UnregisterPeer(uint32_t peerId);

   bool IsPeerRegistered(uint32_t peerId) const
   {
      return m_peerInfoMap.Find(peerId) != m_peerInfoMap.End();
   }

   void SendNetworkMessage(uint32_t peerId, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream);

   void ProcessPacket(uint32_t peerId, InputMemoryBitStream& inputStream);
   void SendOutgoingPackets(const GameTimer& gt);
   void ProcessMessages();
   void ResendNetworkMessage(uint32_t peerId, const MessageToSend& msg);

private:
   NetworkProtocolsManager m_protocolsManager;

   HashMap<uint32_t, PeerInfo> m_peerInfoMap;

   Deque<int> m_processedMessagesIds;
};

} // namespace BIEngine
