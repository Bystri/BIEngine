#pragma once

#include "../Serialization.h"
#include "ObjectReplication.h"

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
        m_masterPeerId(INVALID_PEER_ID),
        m_networkId(0)
   {
   }

   ReplicationHeader(ReplicationAction ra, PeerId masterPeerId, uint32_t networkId, SharedPtr<ReplicationObject> pReplicationObject = nullptr)
      : m_replicationAction(ra),
        m_masterPeerId(masterPeerId),
        m_networkId(networkId),
        m_pReplicationObject(pReplicationObject),
        m_classId(pReplicationObject ? pReplicationObject->GetClassType() : 0)
   {
   }

   ReplicationAction GetReplicationAction() const { return m_replicationAction; };

   PeerId GetMasterPeerId() const { return m_masterPeerId; }

   uint32_t GetNetworkId() const { return m_networkId; }

   uint32_t GetClassId() const { return m_classId; }

   void Write(OutputMemoryBitStream& stream) const;
   bool Read(InputMemoryBitStream& stream);

   InputMemoryBitStream GetPayloadStream() const
   {
      return InputMemoryBitStream(m_pPayload, m_payloadBitCount);
   }

public:
   SharedPtr<ReplicationObject> m_pReplicationObject;
   ReplicationAction m_replicationAction;
   PeerId m_masterPeerId;
   uint32_t m_networkId;
   uint32_t m_classId;
   SharedPtr<char> m_pPayload;
   uint32_t m_payloadBitCount = 0;
};

} // namespace BIEngine
