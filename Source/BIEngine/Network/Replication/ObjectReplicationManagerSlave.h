#pragma once

#include <memory>

#include "ObjectReplication.h"
#include "ReplicationActionReader.h"

namespace BIEngine {

class ObjectReplicationManagerSlave {
public:
   ObjectReplicationManagerSlave()
      : m_pLinkingContext(std::make_shared<NewtworkObjectLinkingContexts>()), m_pReplicationActionReader(std::make_unique<ReplicationActionReader>(m_pLinkingContext))
   {
   }

   void ProcessPacket(InputMemoryBitStream& stream);

private:
   std::shared_ptr<NewtworkObjectLinkingContexts> m_pLinkingContext;
   std::unique_ptr<ReplicationActionReader> m_pReplicationActionReader;
};

} // namespace BIEngine
