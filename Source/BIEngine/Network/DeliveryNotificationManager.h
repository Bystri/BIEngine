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

   void Write(OutputMemoryBitStream& packet) const;
   void Read(InputMemoryBitStream& packet);

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
   InFlightPacket(PacketSequenceNumber sequenceNumber, float timeDispatched)
      : m_sequenceNumber(sequenceNumber), m_timeDispatched(timeDispatched)
   {
   }

   PacketSequenceNumber GetSequenceNumber() const
   {
      return m_sequenceNumber;
   }

   float GetTimeDispatched() const
   {
      return m_timeDispatched;
   }

   void SetTransmissionData(TransmissionDataPtr pTransmissionData)
   {
      m_transmissionDatas.PushBack(pTransmissionData);
   }

   void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const;
   void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;

private:
   PacketSequenceNumber m_sequenceNumber;
   float m_timeDispatched;
   DynamicArray<TransmissionDataPtr> m_transmissionDatas;
};

class DeliveryNotificationManager {
public:
   const Deque<InFlightPacket>& GetInFlightPackets() const
   {
      return m_inFlightPackets;
   }

   inline InFlightPacket* WriteState(OutputMemoryBitStream& outputStream, const GameTimer& gt)
   {
      InFlightPacket* toRet = WriteSequenceNumber(outputStream, gt);
      WritePendingAcks(outputStream);

      return toRet;
   }

   inline bool ReadAndProcessState(InputMemoryBitStream& inputStream)
   {
      bool toRet = ProcessSequenceNumber(inputStream);
      ProcessAcks(inputStream);

      return toRet;
   }

   void ProcessTimedOutPackets(const GameTimer& gt);
   
private:
   InFlightPacket* WriteSequenceNumber(OutputMemoryBitStream& packet, const GameTimer& gt);
   bool ProcessSequenceNumber(InputMemoryBitStream& packet);

   void AddPendingAck(PacketSequenceNumber sequenceNumber);
   void WritePendingAcks(OutputMemoryBitStream& packet);
   void ProcessAcks(InputMemoryBitStream& packet);

   void HandlePacketDeliveryFailure(const InFlightPacket& flightPacket);
   void HandlePacketDeliverySuccess(const InFlightPacket& flightPacket);

private:
   Deque<InFlightPacket> m_inFlightPackets;
   Queue<AckRange> m_pendingAcks;

   PacketSequenceNumber m_nextOutgoingSequenceNumber = 0;
   PacketSequenceNumber m_nextExpectedSequenceNumber = 0;
   uint16_t m_droppedPacketCount = 0;
   uint16_t m_deliveredPacketCount = 0;
};

} // namespace BIEngine
