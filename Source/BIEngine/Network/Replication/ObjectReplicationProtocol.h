#pragma once

#include "../../StdLib/HashSet.h"
#include "../../StdLib/SharedPtr.h"
#include "../../StdLib/UniquePtr.h"
#include "../../StdLib/DynamicArray.h"
#include "../NetworkProtocol.h"
#include "ObjectReplication.h"
#include "ReplicationActionWriter.h"
#include "NetworkObjectCreationRegistry.h"

namespace BIEngine {

class ObjectReplicationProtocolLeader : public NetworkProtocol {
   friend SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t);
   friend void ObjectReplicationDestroy(SharedPtr<ReplicationObject>);

public:
   static const NetworkProtocolType sk_ProtocolType;

   static ObjectReplicationProtocolLeader* Get();

   ObjectReplicationProtocolLeader();
   virtual ~ObjectReplicationProtocolLeader();

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   void AddObjectReplicationPOI(PeerId peerId, SharedPtr<Actor> pActorPOI, float softRadius, float hardRadius);
   void RemoveObjectReplicationPOI(PeerId peerId);

   SharedPtr<ReplicationObject> GetReplicationObject(uint32_t networkId)
   {
      return m_pLinkingContext->GetObj(networkId);
   }

#ifndef _RETAIL
   void DrawDbgDiagnostics() const;
#endif

protected:
   virtual void RegisterPeer(PeerId peerId) override;
   virtual void UnregisterPeer(PeerId peerId) override;

   void UpdateReplicatedObjectsState();
   virtual void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   void AddReplicationObject(SharedPtr<ReplicationObject> pObj);
   void DestroyReplicationObject(SharedPtr<ReplicationObject> pObj);

   void SendStateMsgToClient(PeerId peerId, NetworkMessagesManager* pNetworkMessagesManager);

private:
   struct ReplicationRelevancyInfo {
      HashSet<uint32_t> replicatedObjsSet;
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

class ObjectReplicationProtocolFollower : public NetworkProtocol {
public:
   static const NetworkProtocolType sk_ProtocolType;

   static ObjectReplicationProtocolFollower* Get();

   ObjectReplicationProtocolFollower();
   virtual ~ObjectReplicationProtocolFollower();

   SharedPtr<ReplicationObject> GetReplicationObject(uint32_t networkId)
   {
      return m_pLinkingContext->GetObj(networkId);
   }

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

protected:
    virtual void ReceiveMessage(PeerId peerId, InputMemoryBitStream& stream) override;

private:
   bool ProcessReplicationHeader(InputMemoryBitStream& stream);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;
};

} // namespace BIEngine
