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

   Serialize(stream, m_networkId);
   if (m_replicationAction != ReplicationAction::Destroy) {
      Serialize(stream, m_classId);
   }
}

void ReplicationHeader::Read(InputMemoryBitStream& stream)
{
   uint32_t repAct = 0;
   Deserialize(stream, repAct, GetRequiredBits<static_cast<int>(ReplicationAction::MAX)>::Value);
   m_replicationAction = static_cast<ReplicationAction>(repAct);

   Deserialize(stream, m_networkId);
   if (m_replicationAction != ReplicationAction::Destroy) {
      Deserialize(stream, m_classId);
   }
};

} // namespace BIEngine
