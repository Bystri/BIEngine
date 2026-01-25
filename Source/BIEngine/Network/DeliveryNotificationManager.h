#pragma once

#include "Serialization.h"
#include "../StdLib/Queue.h"
#include "../StdLib/HashMap.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

using PacketSequenceNumber = uint16_t;

class AckRange {
public:
   AckRange()
   {
   }

   AckRange(PacketSequenceNumber start)
      : m_start(start)
   {
   }

   PacketSequenceNumber GetStart() const
   {
      return m_start;
   }

   PacketSequenceNumber GetCount() const
   {
      return m_count;
   }

   inline bool ExtendIfShould(PacketSequenceNumber sequenceNumber)
   {
      if (sequenceNumber == m_start + m_count) {
         ++m_count;
         return true;
      } else {
         return false;
      }
   }

   void Write(OutputMemoryBitStream& packet) const
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

   void Read(InputMemoryBitStream& packet)
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

private:
   PacketSequenceNumber m_start = 0;
   PacketSequenceNumber m_count = 1;
};

class DeliveryNotificationManager;

class TransmissionData {
public:
   virtual void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const {};
   virtual void HandleDeliverySuccess(DeliveryNotificationManager* pDeliveryNotificationManager) const {};
};

using TransmissionDataPtr = SharedPtr<TransmissionData>;

class InFlightPacket {
public:
   InFlightPacket(PacketSequenceNumber sequenceNumber)
      : m_sequenceNumber(sequenceNumber)
   {
   }

   PacketSequenceNumber GetSequenceNumber() const
   {
      return m_sequenceNumber;
   }

   void SetTransmissionData(int key, TransmissionDataPtr pTransmissionData)
   {
      m_transmissionDataMap[key] = pTransmissionData;
   }

   const TransmissionDataPtr GetTransmissionData(int key) const
   {
      auto it = m_transmissionDataMap.Find(key);
      return (it != m_transmissionDataMap.End()) ? it->second : nullptr;
   }

   void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const
   {
      for (const auto& pair : m_transmissionDataMap) {
         pair.second->HandleDeliveryFailure(pDeliveryNotificationManager);
      }
   }

   void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
   {
      for (const auto& pair : m_transmissionDataMap) {
         pair.second->HandleDeliverySuccess(inDeliveryNotificationManager);
      }
   }

private:
   PacketSequenceNumber m_sequenceNumber;
   float m_timeDispatched;
   HashMap<int, TransmissionDataPtr> m_transmissionDataMap;
};

class DeliveryNotificationManager {
public:
   const Deque<InFlightPacket>& GetInFlightPackets() const
   {
      return m_inFlightPackets;
   }

   inline InFlightPacket* WriteState(OutputMemoryBitStream& outputStream)
   {
      InFlightPacket* toRet = WriteSequenceNumber(outputStream);
      WritePendingAcks(outputStream);

      return toRet;
   }

   inline bool ReadAndProcessState(InputMemoryBitStream& inputStream)
   {
      bool toRet = ProcessSequenceNumber(inputStream);
      ProcessAcks(inputStream);

      return toRet;
   }


private:
   InFlightPacket* WriteSequenceNumber(OutputMemoryBitStream& packet)
   {
      const PacketSequenceNumber sequenceNumber = m_nextOutgoingSequenceNumber++;
      Serialize(packet, sequenceNumber);

      m_inFlightPackets.EmplaceBack(sequenceNumber);
      return &m_inFlightPackets.Back();
   }

   bool ProcessSequenceNumber(InputMemoryBitStream& packet)
   {
      PacketSequenceNumber sequenceNumber;
      Deserialize(packet, sequenceNumber);
      if (sequenceNumber == m_nextExpectedSequenceNumber) {
         // is this expected? add ack to the pending list and process packet
         m_nextExpectedSequenceNumber = sequenceNumber + 1;
         AddPendingAck(sequenceNumber);
         return true;
      }
      // is sequence number < current expected? Then silently drop old packet.
      else if (sequenceNumber < m_nextExpectedSequenceNumber) {
         return false;
      }
      // otherwise, we missed some packets
      else if (sequenceNumber > m_nextExpectedSequenceNumber) {
         // consider all skipped packets as dropped, so
         // our next expected packet comes after this one ...
         m_nextExpectedSequenceNumber = sequenceNumber + 1;
         // add an ack for the packet and process it
         // when the sender detects break it acks, it can resend
         AddPendingAck(sequenceNumber);
         return true;
      }
   }

   void AddPendingAck(PacketSequenceNumber sequenceNumber)
   {
      if (m_pendingAcks.Size() == 0 || !m_pendingAcks.Back().ExtendIfShould(sequenceNumber)) {
         m_pendingAcks.Emplace(sequenceNumber);
      }
   }

   void WritePendingAcks(OutputMemoryBitStream& packet)
   {
      const bool hasAcks = (m_pendingAcks.Size() > 0);
      Serialize(packet, hasAcks);
      if (hasAcks) {
         m_pendingAcks.Front().Write(packet);
         m_pendingAcks.Pop();
      }
   }

   void ProcessAcks(InputMemoryBitStream& packet)
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

   /*
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
   */

   void HandlePacketDeliveryFailure(const InFlightPacket& flightPacket)
   {
      ++m_droppedPacketCount;
      flightPacket.HandleDeliveryFailure(this);
   }

   void HandlePacketDeliverySuccess(const InFlightPacket& flightPacket)
   {
      ++m_deliveredPacketCount;
      flightPacket.HandleDeliverySuccess(this);
   }

private:
   Deque<InFlightPacket> m_inFlightPackets;
   Queue<AckRange> m_pendingAcks;

   PacketSequenceNumber m_nextOutgoingSequenceNumber = 0;
   PacketSequenceNumber m_nextExpectedSequenceNumber = 0;
   uint16_t m_droppedPacketCount = 0;
   uint16_t m_deliveredPacketCount = 0;
};

} // namespace BIEngine
