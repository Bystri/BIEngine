#pragma once

#include "../../StdLib/SharedPtr.h"
#include "../../StdLib/UniquePtr.h"
#include "../../StdLib/DynamicArray.h"
#include "ObjectReplication.h"
#include "ReplicationActionWriter.h"
#include "NetworkObjectCreationRegistry.h"
#include "../NetworkManager.h"

namespace BIEngine {

class ObjectReplicationManagerMaster {
   friend SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t);

public:
   static ObjectReplicationManagerMaster* Get();

   void AddClient(SharedPtr<Peer> pPeer);

   void SendPacket(NetworkManager* pNetworkManager);

   void OnUpdate();

private:
   ObjectReplicationManagerMaster();
   virtual ~ObjectReplicationManagerMaster();

   void AddReplicationObject(SharedPtr<ReplicationObject> pObj);

   void SendStatePacketToClient(int peerIdx, NetworkManager* pNetworkManager);

private:
   SharedPtr<NewtworkObjectLinkingContexts> m_pLinkingContext;

   DynamicArray<SharedPtr<Peer>> m_pPeers;
   DynamicArray<UniquePtr<ReplicationActionWriter>> m_pReplicationManagersPerPeer;
   DynamicArray<OutputMemoryBitStream> m_replicationBuffersPerPeer;
   DynamicArray<SharedPtr<ReplicationObject>> m_pReplicationObjects;
};

SharedPtr<ReplicationObject> ObjectReplicationCreate(uint32_t classId);

} // namespace BIEngine
