#pragma once

#include "ObjectReplication.h"
#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ReplicationActionWriter {
public:
   ReplicationActionWriter(SharedPtr<NewtworkObjectLinkingContexts> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ReplicateCreate(OutputMemoryBitStream& stream, SharedPtr<ReplicationObject> pGameObject);
   void ReplicateUpdate(OutputMemoryBitStream& stream, SharedPtr<ReplicationObject> pGameObject);
   void ReplicateDestroy(OutputMemoryBitStream& stream, SharedPtr<ReplicationObject> pGameObject);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
