#pragma once

#include "ReplicationHeader.h"
#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ReplicationActionWriter {
public:
   ReplicationActionWriter(SharedPtr<NewtworkObjectLinkingContexts> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ReplicateCreate(SharedPtr<ReplicationObject> pGameObject);
   void ReplicateUpdate(SharedPtr<ReplicationObject> pGameObject);
   void ReplicateDestroy(SharedPtr<ReplicationObject> pGameObject);

   uint32_t GetNumOfCachedHeaders() const { return m_replicationHeadersBuffer.Size(); }

   void Write(OutputMemoryBitStream& stream)
   {
      if (GetNumOfCachedHeaders() == 0) {
         return;
      }

      const uint32_t numOfHeaders = m_replicationHeadersBuffer.Size();
      stream.WriteBytes(&numOfHeaders, sizeof(uint32_t));

      while (!m_replicationHeadersBuffer.Empty()) {
         ReplicationHeader& header = m_replicationHeadersBuffer.Front();
         header.Write(stream);
         m_replicationHeadersBuffer.Pop();
      }
   }

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
   Queue<ReplicationHeader> m_replicationHeadersBuffer;
};

} // namespace BIEngine
