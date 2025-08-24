#pragma once

#include <memory>

#include "ObjectReplication.h"
#include "ReplicationActionReader.h"
#include "../../StdLib/UniquePtr.h"

namespace BIEngine {

class ObjectReplicationManagerSlave {
public:
   ObjectReplicationManagerSlave()
      : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>()), m_pReplicationActionReader(MakeUnique<ReplicationActionReader>(m_pLinkingContext))
   {
   }

   void ProcessPacket(InputMemoryBitStream& stream);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
   UniquePtr<ReplicationActionReader> m_pReplicationActionReader;
};

} // namespace BIEngine
