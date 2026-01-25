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
   virtual void RegisterPeer(PeerPtr pPeer) override;
   virtual void UnregisterPeer(PeerPtr pPeer) override;

   virtual void OnBeforePacketsSend(NetworkManager* pNetworkManager) override;

private:
   void AddReplicationObject(SharedPtr<ReplicationObject> pObj);

   void SendStateMsgToClient(int peerIdx, NetworkManager* pNetworkManager);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;

   DynamicArray<PeerPtr> m_pPeers;
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
