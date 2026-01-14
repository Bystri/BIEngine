#pragma once

#include "../BIEngine/StdLib/DynamicArray.h"
#include "../BIEngine/Network/NetworkManager.h"
#include "../BIEngine/EventManager/EventManager.h"

class BINetworkManagerClient : public BIEngine::NetworkManager {
   enum class NetworkClientState {
      Uninitialized,
      SayingHello,
      Welcomed
   };

public:
   void Init(const BIEngine::SocketAddress& serverAddress, const BIEngine::String& name);
   void Terminate();

   int GetPlayerId() const { return m_playerId; }

   void SendOutgoingPackets(const BIEngine::GameTimer& gt);

protected:
   virtual void ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress) override;

private:
   void UpdateSayingHello(const BIEngine::GameTimer& gt);
   void SendHelloPacket();

   void HandleWelcomePacket(BIEngine::InputMemoryBitStream& inputStream);

private:
   BIEngine::PeerPtr m_pServerPeer;
   NetworkClientState m_state = NetworkClientState::Uninitialized;

   float m_timeOfLastHello;
   float m_timeOfLastEventPacket;

   int m_playerId;
   BIEngine::String m_name;
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
