#pragma once

#include "../Serialization.h"

namespace BIEngine {

template <typename ReplicatedObject>
class ReplicationUnit {
public:
   virtual ~ReplicationUnit() = default;

   virtual void Init(std::shared_ptr<ReplicatedObject> pReplicatedObject) {}

   virtual bool IsStateChanged() = 0;

   virtual void Read(InputMemoryBitStream& stream) = 0;
   virtual void Write(OutputMemoryBitStream& stream) = 0;
};

} // namespace BIEngine
