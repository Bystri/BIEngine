#pragma once

#include "ObjectReplication.h"
#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ReplicationActionWriter {
public:
   ReplicationActionWriter(std::shared_ptr<NewtworkObjectLinkingContexts> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ReplicateCreate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);
   void ReplicateUpdate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);
   void ReplicateDestroy(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);

private:
   std::shared_ptr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
