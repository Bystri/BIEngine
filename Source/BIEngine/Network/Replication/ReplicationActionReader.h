#pragma once

#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ReplicationActionReader {
public:
   ReplicationActionReader(SharedPtr<NewtworkObjectLinkingContexts> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ProcessReplicationAction(InputMemoryBitStream& stream);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
