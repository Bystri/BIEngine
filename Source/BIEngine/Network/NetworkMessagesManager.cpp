#include "NetworkMessagesManager.h"

namespace BIEngine {

   void NetworkMessagesManager::RegisterPeer(uint32_t peerId, const std::function<void(const OutputMemoryBitStream&)>& sendFunc)
   {
      PeerInfo info;
      info.peerId = peerId;
      info.sendFunc = sendFunc;

      m_peerInfoMap.Insert(peerId, std::move(info));
      m_protocolsManager.RegisterPeer(peerId);
   }

   void NetworkMessagesManager::UnregisterPeer(uint32_t peerId)
   {
      m_protocolsManager.UnregisterPeer(peerId);
      m_peerInfoMap.Erase(peerId);
   }

   void NetworkMessagesManager::SendNetworkMessage(uint32_t peerId, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream)
   {
      auto peerInfoPtr = m_peerInfoMap.Find(peerId);
      if (peerInfoPtr == m_peerInfoMap.End()) {
         Assert(false, "You are trying to send message to an unconnected peer");
         return;
      }

      PeerInfo& peerInfo = peerInfoPtr->second;

      MessageToSend msg(peerInfo.messageId++);

      BIEngine::Serialize(msg.GetBuffer(), protocolType);
      msg.GetBuffer().WriteBits(outputStream.GetBufferPtr().Get(), outputStream.GetBitLength());

      peerInfo.messageQueueToSend.Push(std::move(msg));
   }

   void NetworkMessagesManager::ProcessPacket(uint32_t peerId, InputMemoryBitStream& inputStream)
   {
      auto infoItr = m_peerInfoMap.Find(peerId);

      Assert(infoItr != m_peerInfoMap.End(), "Try to process packet from an unknown peer");

      if (infoItr == m_peerInfoMap.End()) {
         return;
      }

      if (!infoItr->second.deliveryNotificationManager.ReadAndProcessState(inputStream)) {
          return;
      }

      uint32_t messagesCnt;
      Deserialize(inputStream, messagesCnt);

      while (messagesCnt--) {
         MessageToRead& msg = infoItr->second.messageQueueToRead.EmplaceBack();
         msg.Read(inputStream);
      }

      Sort(infoItr->second.messageQueueToRead.Begin(), infoItr->second.messageQueueToRead.End());
   }

   void NetworkMessagesManager::SendOutgoingPackets(const GameTimer& gt)
   {
      m_protocolsManager.OnBeforePacketsSend(this);

      for (auto& peerInfo : m_peerInfoMap) {
         PeerInfo& info = peerInfo.second;

         info.deliveryNotificationManager.ProcessTimedOutPackets(gt);

         if (info.messageQueueToSend.Empty()) {
            continue;
         }

         OutputMemoryBitStream packet;

         InFlightPacket* inFlightPacket = peerInfo.second.deliveryNotificationManager.WriteState(packet, gt);

         const uint32_t messagesCnt = info.messageQueueToSend.Size();
         Serialize(packet, messagesCnt);

         //Logger::WriteMsgLog("Prepare to send %d messages", (int)messagesCnt);

         while (!info.messageQueueToSend.Empty()) {
            const MessageToSend& msg = info.messageQueueToSend.Front();

            msg.Write(packet);

            TransmissionDataPtr pData = MakeShared<NetworkTransmissionData>(info.peerId, msg, this);
            inFlightPacket->SetTransmissionData(pData);

            info.messageQueueToSend.Pop();
         }

         info.sendFunc(packet);
      }
   }

   void NetworkMessagesManager::ProcessMessages()
   {
      for (auto& peerInfo : m_peerInfoMap) {
         while (!peerInfo.second.messageQueueToRead.Empty()) {
            MessageToRead& msg = peerInfo.second.messageQueueToRead[0];

            static bool dbgBool = false;

            if (msg.GetId() < peerInfo.second.expectedMessageId) {
               peerInfo.second.messageQueueToRead.Erase(peerInfo.second.messageQueueToRead.Begin());
               continue;
            }

            if (msg.GetId() > peerInfo.second.expectedMessageId && !dbgBool) {
               dbgBool = true;
               break;
            }

            dbgBool = true;

            m_processedMessagesIds.PushBack(msg.GetId());

            ++peerInfo.second.expectedMessageId;

            uint32_t packetType;
            Deserialize(msg.GetBuffer(), packetType);

            m_protocolsManager.ReceiveMeessage(packetType, msg.GetBuffer());

            peerInfo.second.messageQueueToRead.Erase(peerInfo.second.messageQueueToRead.Begin());
         }
      }
   }

   void NetworkMessagesManager::ResendNetworkMessage(uint32_t peerId, const MessageToSend& msg)
   {
      auto peerInfoPtr = m_peerInfoMap.Find(peerId);
      if (peerInfoPtr == m_peerInfoMap.End()) {
         Assert(false, "You are trying to send message to an unconnected peer");
         return;
      }

      PeerInfo& peerInfo = peerInfoPtr->second;
      peerInfo.messageQueueToSend.Push(msg);
   }

} // namespace BIEngine
