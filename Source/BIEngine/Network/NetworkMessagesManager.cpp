#include "NetworkMessagesManager.h"

#include <imgui.h>

namespace BIEngine {

void NetworkMessagesManager::RegisterPeer(PeerId peerId, const GameTimer& gt, const std::function<void(const OutputMemoryBitStream&)>& sendFunc)
{
   PeerInfo info(gt);
   info.peerId = peerId;
   info.sendFunc = sendFunc;

   m_peerInfoMap.Insert(peerId, std::move(info));
   m_protocolsManager.RegisterPeer(peerId);
}

void NetworkMessagesManager::UnregisterPeer(PeerId peerId)
{
   m_protocolsManager.UnregisterPeer(peerId);
   m_peerInfoMap.Erase(peerId);
}

float NetworkMessagesManager::GetRttForPeer(PeerId peerId) const
{
   auto itr = m_peerInfoMap.Find(peerId);
   if (itr == m_peerInfoMap.End()) {
      Assert(false, "Trying to get RTT for unknown peerId [%u]", peerId);
      return 0.0f;
   }

   return itr->second.m_weightedRtt.GetValue();
}

void NetworkMessagesManager::SendNetworkMessage(PeerId peerId, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream)
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

void NetworkMessagesManager::ProcessPacket(PeerId peerId, InputMemoryBitStream& inputStream, const GameTimer& gt)
{
   auto infoItr = m_peerInfoMap.Find(peerId);

   Assert(infoItr != m_peerInfoMap.End(), "Try to process packet from an unknown peer");

   if (infoItr == m_peerInfoMap.End()) {
      return;
   }

   uint32_t numberOfTimes;
   Deserialize(inputStream, numberOfTimes);
   for (int i = 0; i < numberOfTimes; ++i) {
      float timePacketWasSendByThisPeer;
      // TODO: Quant
      Deserialize(inputStream, timePacketWasSendByThisPeer);
      infoItr->second.m_weightedRtt.Update(gt, gt.TotalTime() - timePacketWasSendByThisPeer);
   }

   float timePacketWasSend;
   // TODO: Quant
   Deserialize(inputStream, timePacketWasSend);
   infoItr->second.m_timesOfGotPackets.PushBack(timePacketWasSend);

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

      OutputMemoryBitStream packet;
      const uint32_t timesOfGotPacketsSize = info.m_timesOfGotPackets.Size();
      Serialize(packet, timesOfGotPacketsSize);
      for (int i = 0; i < timesOfGotPacketsSize; ++i) {
         // TODO: Quant
         Serialize(packet, info.m_timesOfGotPackets[i]);
      }

      info.m_timesOfGotPackets.Clear();

      // TODO: Quant
      Serialize(packet, gt.TotalTime());

      InFlightPacket* inFlightPacket = peerInfo.second.deliveryNotificationManager.WriteState(packet, gt);

      const uint32_t messagesCnt = info.messageQueueToSend.Size();
      Serialize(packet, messagesCnt);

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

void NetworkMessagesManager::ResendNetworkMessage(PeerId peerId, const MessageToSend& msg)
{
   auto peerInfoPtr = m_peerInfoMap.Find(peerId);
   if (peerInfoPtr == m_peerInfoMap.End()) {
      Assert(false, "You are trying to send message to an unconnected peer");
      return;
   }

   PeerInfo& peerInfo = peerInfoPtr->second;
   peerInfo.messageQueueToSend.Push(msg);
}

#ifndef _RETAIL
void NetworkMessagesManager::DrawDbgDiagnostics()
{
   ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_Always);

   if (!ImGui::Begin("Network info")) {
      ImGui::End();
      return;
   }

   for (auto& peerInfo : m_peerInfoMap) {
      ImGui::Text("Peer: [%d]; Rtt %f", peerInfo.first, peerInfo.second.m_weightedRtt.GetValue());
   }

   ImGui::End();
}
#endif

} // namespace BIEngine
