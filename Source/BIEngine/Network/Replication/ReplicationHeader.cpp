#include "ReplicationHeader.h"

namespace BIEngine {

template <int tValue, int tBits>
struct GetRequiredBitsHelper {
   enum {
      Value = GetRequiredBitsHelper<(tValue >> 1), tBits + 1>::Value
   };
};

template <int tBits>
struct GetRequiredBitsHelper<0, tBits> {
   enum { Value = tBits };
};

template <int tValue>
struct GetRequiredBits {
   enum { Value = GetRequiredBitsHelper<tValue, 0>::Value };
};

void ReplicationHeader::Write(OutputMemoryBitStream& stream) const
{
   stream.WriteBits(static_cast<uint32_t>(m_replicationAction), GetRequiredBits<static_cast<int>(ReplicationAction::MAX)>::Value);

   Serialize(stream, m_masterPeerId);
   Serialize(stream, m_networkId);
   if (m_replicationAction == ReplicationAction::Destroy) {
      return;
   }

   Serialize(stream, GetClassId());

   OutputMemoryBitStream payloadStream;
   m_pReplicationObject->Write(payloadStream, m_replicationAction == ReplicationAction::Create);

   const uint32_t payloadBitCount = payloadStream.GetBitLength();
   Serialize(stream, payloadBitCount);
   stream.WriteBits(payloadStream.GetBufferPtr().Get(), payloadBitCount);
}

bool ReplicationHeader::Read(InputMemoryBitStream& stream)
{
   uint32_t repAct = 0;
   Deserialize(stream, repAct, GetRequiredBits<static_cast<int>(ReplicationAction::MAX)>::Value);
   if (stream.HasReadError() || repAct >= static_cast<uint32_t>(ReplicationAction::MAX)) {
      return false;
   }

   m_replicationAction = static_cast<ReplicationAction>(repAct);

   Deserialize(stream, m_masterPeerId);
   Deserialize(stream, m_networkId);
   if (stream.HasReadError()) {
      return false;
   }

   if (m_replicationAction == ReplicationAction::Destroy) {
      return true;
   }

   Deserialize(stream, m_classId);
   Deserialize(stream, m_payloadBitCount);
   if (stream.HasReadError()) {
      return false;
   }

   if (m_payloadBitCount > stream.GetRemainingBitCount()) {
      return false;
   }

   if (m_payloadBitCount == 0) {
      return true;
   }

   const uint32_t payloadByteCount = (m_payloadBitCount + 7) >> 3;
   m_pPayload = SharedPtr<char>(static_cast<char*>(std::malloc(payloadByteCount)), std::free);
   if (m_pPayload == nullptr) {
      return false;
   }

   stream.ReadBits(m_pPayload.Get(), m_payloadBitCount);
   return !stream.HasReadError();
}

} // namespace BIEngine
