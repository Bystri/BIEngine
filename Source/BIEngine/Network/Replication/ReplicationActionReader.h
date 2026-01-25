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
   void ProcessReplicationActions(InputMemoryBitStream& stream);

private:
   void ProcessReplicationHeader(InputMemoryBitStream& stream);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
