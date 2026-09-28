#pragma once

#include "../Serialization.h"

namespace BIEngine {

using ReplicationUnitTypeId = uint32_t;

class ReplicationObject;

template <typename ReplicatedObject>
class ReplicationUnit {
public:
   virtual ReplicationUnitTypeId GetTypeId() const = 0;

   virtual ~ReplicationUnit() = default;

   virtual void Init(ReplicationObject* pReplicationObject, SharedPtr<ReplicatedObject> pReplicatedObject) 
   {
      m_pOwner = pReplicationObject;
   }

   virtual bool IsStateChanged() = 0;

   virtual void Read(InputMemoryBitStream& stream) = 0;
   virtual void Write(OutputMemoryBitStream& stream) = 0;

   ReplicationObject* GetOwner() const
   {
      return m_pOwner;
   }

 private:
   ReplicationObject* m_pOwner;
};

} // namespace BIEngine
