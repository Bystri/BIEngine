#pragma once

#include "../../StdLib/SharedPtr.h"
#include "../../StdLib/UniquePtr.h"
#include "../../StdLib/DynamicArray.h"
#include "../NetworkProtocol.h"
#include "ObjectReplication.h"
#include "ReplicationActionWriter.h"
#include "ReplicationActionReader.h"
#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ObjectReplicationProtocolWriter : public NetworkProtocolWriter {
   friend SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t);

public:
   static const NetworkProtocolType sk_ProtocolType;

   static ObjectReplicationProtocolWriter* Get();

   ObjectReplicationProtocolWriter();
   virtual ~ObjectReplicationProtocolWriter();

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   void OnUpdate();

protected:
   virtual void RegisterPeer(uint32_t peerId) override;
   virtual void UnregisterPeer(uint32_t peerId) override;

   virtual void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   void AddReplicationObject(SharedPtr<ReplicationObject> pObj);

   void SendStateMsgToClient(uint32_t peerId, NetworkMessagesManager* pNetworkMessagesManager);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;

   DynamicArray<uint32_t> m_pPeers;
   DynamicArray<UniquePtr<ReplicationActionWriter>> m_pReplicationManagersPerPeer;
   DynamicArray<SharedPtr<ReplicationObject>> m_pReplicationObjects;
};

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId);

class ObjectReplicationProtocolReader : public NetworkProtocolReader {
public:
   static const NetworkProtocolType sk_ProtocolType;

   ObjectReplicationProtocolReader()
      : m_pLinkingContext(MakeShared<NewtworkObjectLinkingContexts>()), m_pReplicationActionReader(MakeUnique<ReplicationActionReader>(m_pLinkingContext))
   {
   }

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   virtual void ReceiveMessage(InputMemoryBitStream& stream) override;

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
   UniquePtr<ReplicationActionReader> m_pReplicationActionReader;
};

} // namespace BIEngine
