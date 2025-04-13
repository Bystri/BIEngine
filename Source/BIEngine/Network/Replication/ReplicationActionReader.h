#pragma once

#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ReplicationActionReader {
public:
   ReplicationActionReader(std::shared_ptr<NewtworkObjectLinkingContexts> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ProcessReplicationAction(InputMemoryBitStream& stream);

private:
   std::shared_ptr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
