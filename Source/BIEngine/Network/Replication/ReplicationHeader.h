#pragma once

#include "../Serialization.h"

namespace BIEngine {

enum class ReplicationAction : uint8_t {
   Create,
   Update,
   Destroy,
   MAX
};

class ReplicationHeader {
public:
   ReplicationHeader()
      : m_replicationAction(ReplicationAction::Create),
        m_networkId(0),
        m_classId(0)
   {
   }

   ReplicationHeader(ReplicationAction ra, uint32_t networkId, uint32_t classId = 0)
      : m_replicationAction(ra),
        m_networkId(networkId),
        m_classId(classId)
   {
   }

   ReplicationAction GetReplicationAction() const { return m_replicationAction; };

   uint32_t GetNetworkId() const { return m_networkId; }

   uint32_t GetClassId() const { return m_classId; }

   void Write(OutputMemoryBitStream& stream) const;
   void Read(InputMemoryBitStream& stream);

public:
   ReplicationAction m_replicationAction;
   uint32_t m_networkId;
   uint32_t m_classId;
};

} // namespace BIEngine
