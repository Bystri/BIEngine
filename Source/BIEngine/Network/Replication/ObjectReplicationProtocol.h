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
   friend void ObjectReplicationDestroy(SharedPtr<ReplicationObject>);

public:
   static const NetworkProtocolType sk_ProtocolType;

   static ObjectReplicationProtocolWriter* Get();

   ObjectReplicationProtocolWriter();
   virtual ~ObjectReplicationProtocolWriter();

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   void AddObjectReplicationPOI(PeerId peerId, SharedPtr<Actor> pActorPOI, float softRadius, float hardRadius);
   void RemoveObjectReplicationPOI(PeerId peerId);

   void OnUpdate();

#ifndef _RETAIL
   void DrawDbgDiagnostics() const;
#endif

protected:
   virtual void RegisterPeer(PeerId peerId) override;
   virtual void UnregisterPeer(PeerId peerId) override;

   virtual void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   void AddReplicationObject(SharedPtr<ReplicationObject> pObj);
   void DestroyReplicationObject(SharedPtr<ReplicationObject> pObj);

   void SendStateMsgToClient(PeerId peerId, NetworkMessagesManager* pNetworkMessagesManager);

private:
   struct ReplicationRelevancyInfo {
      SharedPtr<Actor> pActorPOI;
      float softRadius;
      float hardRadius;
   };

   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;

   DynamicArray<PeerId> m_pPeers;
   DynamicArray<UniquePtr<ReplicationActionWriter>> m_pReplicationManagersPerPeer;
   DynamicArray<SharedPtr<ReplicationObject>> m_pReplicationObjects;
   HashMap<PeerId, ReplicationRelevancyInfo> m_relevancyInfo;
};

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId);
void ObjectReplicationDestroy(SharedPtr<ReplicationObject> pGameObject);

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
