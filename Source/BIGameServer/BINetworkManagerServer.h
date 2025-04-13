#pragma once

#include <unordered_map>

#include "../BIEngine/Network/NetworkManager.h"
#include "../BIEngine/Network/Replication/ObjectReplication.h"

class BINetworkManagerServer : public BIEngine::NetworkManager {
   using AddressToClientMap = std::unordered_map<BIEngine::SocketAddress, BIEngine::PeerPtr>;
   using IntToClientMap = std::unordered_map<int, BIEngine::PeerPtr>;

public:
   bool Init(uint16_t port);

   void SendOutgoingPackets();

protected:
   virtual void ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress) override;

   /*
   virtual void HandleConnectionReset(const SocketAddress& inFromAddress) override;

   void CheckForDisconnects();

   inline GameObjectPtr RegisterAndReturn(GameObject* inGameObject);
   void UnregisterGameObject(GameObject* inGameObject);
   void SetStateDirty(int inNetworkId, uint32_t inDirtyState);

   void RespawnCats();

   ClientProxyPtr GetClientProxy(int inPlayerId) const;
   */

private:
   void HandlePacketFromNewClient(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress);
   void ProcessPacket(BIEngine::PeerPtr clientProxy, BIEngine::InputMemoryBitStream& inputStream);

   void SendWelcomePacket(BIEngine::PeerPtr clientProxy);

   void HandleEventPacket(BIEngine::PeerPtr pClientProxy, BIEngine::InputMemoryBitStream& inputStream);

private:
   AddressToClientMap m_addressToClientMap;
   uint32_t nextClientId = 0u;

   /*
   void AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream);

   void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy);

   void HandleClientDisconnected(ClientProxyPtr inClientProxy);

   float mTimeOfLastSatePacket;
   float mTimeBetweenStatePackets;
   float mClientDisconnectTimeout;
   */
};

/*
 inline GameObjectPtr NetworkManagerServer::RegisterAndReturn(GameObject* inGameObject)
{
   GameObjectPtr toRet(inGameObject);
   RegisterGameObject(toRet);
   return toRet;
}
*/
