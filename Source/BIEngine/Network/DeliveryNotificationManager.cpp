#include "DeliveryNotificationManager.h"

namespace BIEngine {

/*AckRange*/

void AckRange::Write(OutputMemoryBitStream& packet) const
{
   Serialize(packet, m_start);
   bool hasCount = m_count > 1;
   Serialize(packet, hasCount);
   if (hasCount) {
      // let's assume you want to ack max of 8 bits...
      uint32_t countMinusOne = m_count - 1;
      uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
      Serialize(packet, countToAck);
   }
}

void AckRange::Read(InputMemoryBitStream& packet)
{
   Deserialize(packet, m_start);
   bool hasCount;
   Deserialize(packet, hasCount);
   if (hasCount) {
      uint8_t countMinusOne;
      Deserialize(packet, countMinusOne);
      m_count = countMinusOne + 1;
   } else {
      // default!
      m_count = 1;
   }
}

/*InFlightPacket*/

void InFlightPacket::HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const
{
   for (const auto& data : m_transmissionDatas) {
      data->HandleDeliveryFailure(pDeliveryNotificationManager);
   }
}

void InFlightPacket::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
   for (const auto& data : m_transmissionDatas) {
      data->HandleDeliverySuccess(inDeliveryNotificationManager);
   }
}

/*DeliveryNotificationManager*/

void DeliveryNotificationManager::ProcessTimedOutPackets(const GameTimer& gt)
{
   const float ACK_TIMEOUT = 30.0f;

   float timeoutTime = gt.TotalTime() - ACK_TIMEOUT;
   while (!m_inFlightPackets.Empty()) {
      // packets are sorted, so all timed out packets must be at front
      const auto& nextInFlightPacket = m_inFlightPackets.Front();
      if (nextInFlightPacket.GetTimeDispatched() < timeoutTime) {
         HandlePacketDeliveryFailure(nextInFlightPacket);
         m_inFlightPackets.PopFront();
      } else {
         // no packets beyond could be timed out
         break;
      }
   }
}

InFlightPacket* DeliveryNotificationManager::WriteSequenceNumber(OutputMemoryBitStream& packet, const GameTimer& gt)
{
   const PacketSequenceNumber sequenceNumber = m_nextOutgoingSequenceNumber++;
   Serialize(packet, sequenceNumber);

   m_inFlightPackets.EmplaceBack(sequenceNumber, gt.TotalTime());
   return &m_inFlightPackets.Back();
}

bool DeliveryNotificationManager::ProcessSequenceNumber(InputMemoryBitStream& packet)
{
   PacketSequenceNumber sequenceNumber;
   Deserialize(packet, sequenceNumber);
   AddPendingAck(sequenceNumber);

   return true;
}

void DeliveryNotificationManager::AddPendingAck(PacketSequenceNumber sequenceNumber)
{
   if (m_pendingAcks.Size() == 0 || !m_pendingAcks.Back().ExtendIfShould(sequenceNumber)) {
      m_pendingAcks.Emplace(sequenceNumber);
   }
}

void DeliveryNotificationManager::WritePendingAcks(OutputMemoryBitStream& packet)
{
   const bool hasAcks = (m_pendingAcks.Size() > 0);
   Serialize(packet, hasAcks);
   if (hasAcks) {
      m_pendingAcks.Front().Write(packet);
      m_pendingAcks.Pop();
   }
}

void DeliveryNotificationManager::ProcessAcks(InputMemoryBitStream& packet)
{
   bool hasAcks;
   Deserialize(packet, hasAcks);
   if (hasAcks) {
      AckRange ackRange;
      ackRange.Read(packet);
      // for each InFlightPacket with seq# < start, handle failure...
      PacketSequenceNumber nextAckdSequenceNumber = ackRange.GetStart();
      uint32_t onePastAckdSequenceNumber = nextAckdSequenceNumber + ackRange.GetCount();

      while (nextAckdSequenceNumber < onePastAckdSequenceNumber && !m_inFlightPackets.Empty()) {
         const auto& nextInFlightPacket = m_inFlightPackets.Front();
         // if the packet seq# < ack seq#, we didn't get an ack for it,
         // so it probably wasn't delivered
         PacketSequenceNumber nextInFlightPacketSequenceNumber = nextInFlightPacket.GetSequenceNumber();

         if (nextInFlightPacketSequenceNumber < nextAckdSequenceNumber) {
            // copy this so we can remove it before handling-
            // dependent modules shouldn't find it if seeing what's live
            auto copyOfInFlightPacket = nextInFlightPacket;
            m_inFlightPackets.PopFront();
            HandlePacketDeliveryFailure(copyOfInFlightPacket);
         } else if (nextInFlightPacketSequenceNumber == nextAckdSequenceNumber) {
            HandlePacketDeliverySuccess(nextInFlightPacket);
            // received!
            m_inFlightPackets.PopFront();
            ++nextAckdSequenceNumber;
         } else if (nextInFlightPacketSequenceNumber > nextAckdSequenceNumber) {
            // somehow part of this range was already removed
            //(maybe from timeout) check rest of range
            nextAckdSequenceNumber = nextInFlightPacketSequenceNumber;
         }
      }
   }
}

void DeliveryNotificationManager::HandlePacketDeliveryFailure(const InFlightPacket& flightPacket)
{
   ++m_droppedPacketCount;
   flightPacket.HandleDeliveryFailure(this);
}

void DeliveryNotificationManager::HandlePacketDeliverySuccess(const InFlightPacket& flightPacket)
{
   ++m_deliveredPacketCount;
   flightPacket.HandleDeliverySuccess(this);
}

} // namespace BIEngine
