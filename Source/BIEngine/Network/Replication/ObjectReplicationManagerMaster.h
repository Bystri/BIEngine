#pragma once

#include <memory>
#include <vector>

#include "ObjectReplication.h"
#include "ReplicationActionWriter.h"
#include "NetworkObjectCreationRegistry.h"
#include "../NetworkManager.h"

namespace BIEngine {

class ObjectReplicationManagerMaster {
   friend std::shared_ptr<ReplicationObject> ObjectReplicationCreate(uint32_t);

public:
   static ObjectReplicationManagerMaster* Get();

   void AddClient(std::shared_ptr<Peer> pPeer);

   void SendPacket(NetworkManager* pNetworkManager);

   void OnUpdate();

private:
   ObjectReplicationManagerMaster();
   virtual ~ObjectReplicationManagerMaster();

   void AddReplicationObject(std::shared_ptr<ReplicationObject> pObj);

   void SendStatePacketToClient(int peerIdx, NetworkManager* pNetworkManager);

private:
   std::shared_ptr<NewtworkObjectLinkingContexts> m_pLinkingContext;

   std::vector<std::shared_ptr<Peer>> m_pPeers;
   std::vector<std::unique_ptr<ReplicationActionWriter>> m_pReplicationManagersPerPeer;
   std::vector<OutputMemoryBitStream> m_replicationBuffersPerPeer;
   std::vector<std::shared_ptr<ReplicationObject>> m_pReplicationObjects;
};

std::shared_ptr<ReplicationObject> ObjectReplicationCreate(uint32_t classId);

} // namespace BIEngine
