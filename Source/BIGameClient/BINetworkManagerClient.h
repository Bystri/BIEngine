#pragma once

#include "../BIEngine/Network/NetworkManager.h"
#include "../BIEngine/Network/Replication/ObjectReplicationManagerSlave.h"
#include "../BIEngine/EventManager/EventManager.h"

class BINetworkManagerClient : public BIEngine::NetworkManager {
   enum class NetworkClientState {
      Uninitialized,
      SayingHello,
      Welcomed
   };

public:
   void Init(const BIEngine::SocketAddress& serverAddress, const std::string& name);

   int GetPlayerId() const { return m_playerId; }

   void SendOutgoingPackets(const BIEngine::GameTimer& gt);

protected:
   virtual void ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress) override;

private:
   void UpdateSayingHello(const BIEngine::GameTimer& gt);
   void SendHelloPacket();

   void HandleWelcomePacket(BIEngine::InputMemoryBitStream& inputStream);
   void HandleStatePacket(BIEngine::InputMemoryBitStream& inputStream);

   void StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData);
   void UpdateSendingEventPacket(const BIEngine::GameTimer& gt);
   void SendEventPacket();

private:
   BIEngine::SocketAddress m_serverAddress;
   NetworkClientState m_state = NetworkClientState::Uninitialized;

   float m_timeOfLastHello;
   float m_timeOfLastEventPacket;

   std::unique_ptr<BIEngine::ObjectReplicationManagerSlave> m_pReplicationManager;

   int m_playerId;
   std::string m_name;

   std::vector<BIEngine::IEventDataPtr> m_eventsToSend;
   /*

public:

   float GetLastMoveProcessedByServerTimestamp() const { return mLastMoveProcessedByServerTimestamp; }

private:
   void ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);

   void HandleGameObjectState(InputMemoryBitStream& inInputStream);

   void DestroyGameObjectsInMap(const IntToGameObjectMap& inObjectsToDestroy);

   float mLastMoveProcessedByServerTimestamp;
   */
};
