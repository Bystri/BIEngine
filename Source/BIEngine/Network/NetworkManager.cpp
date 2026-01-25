#include "NetworkManager.h"

#include <imgui.h>

#include "../Utilities/Logger.h"

namespace BIEngine {

bool NetworkManager::InitInternal(uint16_t port)
{
   m_socket = SocketUtil::CreateUdpSocket(SocketAddressFamily::INET);
   const SocketAddress ownAddress(INADDR_ANY, port);
   m_socket->Bind(ownAddress);

   Logger::WriteMsgLog("Initializing NetworkManager at port %d", port);

   if (m_socket == nullptr) {
      return false;
   }

   if (m_socket->SetNonBlockingMode(true) != NO_ERROR) {
      return false;
   }

   return true;
}

void NetworkManager::ProcessIncomingPackets()
{
   ReadIncomingPackets();

   for (auto& peerInfo : m_peerInfoMap) {
      while (!peerInfo.second.messageQueueToRead.Empty()) {
         MessageToRead& msg = peerInfo.second.messageQueueToRead[0];
         if (msg.GetId() < peerInfo.second.expectedMessageId) {
            peerInfo.second.messageQueueToRead.Erase(peerInfo.second.messageQueueToRead.Begin());
            continue;
         }

         if (msg.GetId() > peerInfo.second.expectedMessageId) {
            break;
         }

         m_processedMessagesIds.PushBack(msg.GetId());

         ++peerInfo.second.expectedMessageId;

         uint32_t packetType;
         Deserialize(msg.GetBuffer(), packetType);

         m_protocolsManager.ReceiveMeessage(packetType, msg.GetBuffer());

         peerInfo.second.messageQueueToRead.Erase(peerInfo.second.messageQueueToRead.Begin());
      }
   }
}

void NetworkManager::ReadIncomingPackets()
{
   constexpr int packetSize = 1500;
   SharedPtr<char> segment(static_cast<char*>(std::malloc(packetSize)), std::free);
   InputMemoryBitStream inputStream(segment, packetSize * 8);
   SocketAddress fromAddress;

   int receivedPackedCount = 0;

   while (receivedPackedCount < MAX_PACKETS_PER_FRAME_COUNT) {
      int readByteCount = m_socket->ReceiveFrom(segment.Get(), packetSize, fromAddress);
      if (readByteCount == 0) {
         // nothing to read
         break;
      } else if (readByteCount == -WSAECONNRESET) {
         // port closed on other end, so DC this person immediately
         HandleConnectionReset(fromAddress);
      } else if (readByteCount > 0) {
         inputStream.ResetToCapacity(readByteCount);
         ++receivedPackedCount;

         ProcessPacket(inputStream, fromAddress);
      } else {
         // uhoh, error? exit or just keep going?
      }
   }
}

void NetworkManager::SendNetworkMessage(const Peer& peer, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream)
{
   auto peerInfoPtr = m_peerInfoMap.Find(peer.GetId());
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

void NetworkManager::SendMessagesFromQueue(const GameTimer& gt)
{
   for (auto& peerInfo : m_peerInfoMap) {
      PeerInfo& info = peerInfo.second;

      info.deliveryNotificationManager.ProcessTimedOutPackets(gt);

      OutputMemoryBitStream packet;

      InFlightPacket* inFlightPacket = peerInfo.second.deliveryNotificationManager.WriteState(packet, gt);

      int keyOfTransitionData = 0;

      while (!info.messageQueueToSend.Empty()) {
         const MessageToSend& msg = info.messageQueueToSend.Front();

         msg.Write(packet);

         TransmissionDataPtr pData = MakeShared<NetworkTransmissionData>(info.pPeer, msg, this);
         inFlightPacket->SetTransmissionData(keyOfTransitionData++, pData);

         info.messageQueueToSend.Pop();
      }

      m_socket->SendTo(packet.GetBufferPtr().Get(), packet.GetByteLength(), info.pPeer->GetSocketAddress());
   }
}

#ifndef _RETAIL
void NetworkManager::DrawDbgDiagnostics()
{
   ImGui::SetNextWindowSize(ImVec2(550, 650), ImGuiCond_Always);

   if (!ImGui::Begin("Network info")) {
      ImGui::End();
      return;
   }

   for (auto& peerInfo : m_peerInfoMap) {
      static ImGuiTableFlags flags = ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders;

      const bool openInFLightPackets = ImGui::CollapsingHeader("In flight packets", ImGuiTreeNodeFlags_DefaultOpen);
      if (openInFLightPackets && ImGui::BeginTable("In flight packets", 2, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 5))) {
         ImGui::TableSetupColumn("Seq num");
         ImGui::TableSetupColumn("Time dispatched");
         ImGui::TableHeadersRow();

         for (int i = 0; i < peerInfo.second.deliveryNotificationManager.GetInFlightPackets().Size(); ++i) {
            ImGui::TableNextColumn();
            ImGui::Text("%d", peerInfo.second.deliveryNotificationManager.GetInFlightPackets()[i].GetSequenceNumber());

            ImGui::TableNextColumn();
            ImGui::Text("%f", peerInfo.second.deliveryNotificationManager.GetInFlightPackets()[i].GetTimeDispatched());
         }

         ImGui::EndTable();
      }

      const bool openQueueMessagesToRead = ImGui::CollapsingHeader("Queue messages to read", ImGuiTreeNodeFlags_DefaultOpen);
      if (openQueueMessagesToRead && ImGui::BeginTable("Table msg to read queue", 2, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 5))) {
         ImGui::TableSetupColumn("Seq num");
         ImGui::TableSetupColumn("Expected seq num");
         ImGui::TableHeadersRow();

         for (int i = 0; i < peerInfo.second.messageQueueToRead.Size(); ++i) {
            ImGui::TableNextColumn();
            ImGui::Text("%d", peerInfo.second.messageQueueToRead[i].GetId());

            ImGui::TableNextColumn();
            ImGui::Text("%d", peerInfo.second.expectedMessageId);
         }

         ImGui::EndTable();
      }

      const bool openProcessedMessages = ImGui::CollapsingHeader("Processed messages", ImGuiTreeNodeFlags_DefaultOpen);
      if (openProcessedMessages && ImGui::BeginTable("Table processed messages", 1, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 5))) {
         ImGui::TableSetupColumn("Seq num");
         ImGui::TableHeadersRow();

         for (int i = 0; i < m_processedMessagesIds.Size(); ++i) {
            ImGui::TableNextColumn();
            ImGui::Text("%d", m_processedMessagesIds[i]);
         }

         ImGui::EndTable();
      }
   }

   ImGui::End();
}
#endif

} // namespace BIEngine
