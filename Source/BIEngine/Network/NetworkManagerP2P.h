#include <queue>
#include <memory>
#include <list>
#include <map>
#include <unordered_map>

#include "Socket.h"
#include "Serialization.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class NetworkManager {
public:
   static const uint32_t HELLO_CC = 'HELO';        // sent when first trying to join
   static const uint32_t WELCOME_CC = 'WLCM';      // sent when accepted by master peer
   static const uint32_t NOT_MASTER_PEER = 'NOMP'; // sent as a reply to HELO if it isn't the master peer
   static const uint32_t NOT_JOINABLE_CC = 'NOJN'; // sent as a reply to HELO if the game can't be joined (either full or already started)
   static const uint32_t INTRO_CC = 'INTR';        // sent by new player to all non-master peers after being accepted
   static const uint32_t TURN_CC = 'TURN';         // contains data for a particular turn
   static const uint32_t START_CC = 'STRT';        // notifies peers that the game will be starting soon
   static const int MAX_PACKETS_PER_FRAME_COUNT = 10;

   enum class NetworkManagerState {
      Unitialized,
      Hello,
      Lobby,
      // everything above this should be the pre-game/lobby/connection
      Starting,
      Playing,
      Delay,
   };

   ~NetworkManager();

   static std::unique_ptr<NetworkManager> CreateAsMasterPeer(uint16_t port, const std::string& name);
   static std::unique_ptr<NetworkManager> CreateAsPeer(const SocketAddress& MPAddress, const std::string& name);

   void ProcessIncomingPackets(const float dt);

   void SendOutgoingPackets(const GameTimer& gt);

private:
   NetworkManager();

   void UpdateSayingHello(const float dt);
   void SendHelloPacket();
   void UpdateStarting(const float dt);
   void UpdateSendTurnPacket();
   void TryAdvanceTurn();

public:
   void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt);

private:
   void ProcessPacketsHello(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleNotMPPacket(InputMemoryBitStream& inputStream);
   void HandleWelcomePacket(InputMemoryBitStream& inputStream);
   void ProcessPacketsLobby(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt);
   void HandleHelloPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleIntroPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleStartPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt);
   void ProcessPacketsPlaying(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void HandleTurnPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);
   void ProcessPacketsDelay(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress);

public:
   void HandleConnectionReset(const SocketAddress& fromAddress);

   void SendPacket(const OutputMemoryBitStream& outputStream, const SocketAddress& toAddress);

   void TryStartGame();

   bool IsMasterPeer() const { return m_isMasterPeer; }

   float GetTimeToStart() const { return m_timeToStart; }

   // GameObjectPtr GetGameObject(uint32_t inNetworkId) const;
   // GameObjectPtr RegisterAndReturn(GameObject* inGameObject);
   // void UnregisterGameObject(GameObject* inGameObject);

   NetworkManagerState GetState() const { return m_state; }

   int GetPlayerCount() const { return m_playerCount; }

   int GetTurnNumber() const { return m_turnNumber; }

   int GetSubTurnNumber() const { return m_subTurnNumber; }

   uint32_t GetMyPlayerId() const { return m_playerId; }

private:
   // void AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
   // void RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
   // void RegisterGameObject(GameObjectPtr inGameObject);
   uint32_t GetNewNetworkId();
   // uint32_t ComputeGlobalCRC();

   bool InitAsMasterPeer(uint16_t port, const std::string& name);
   bool InitAsPeer(const SocketAddress& MPAddress, const std::string& name);
   bool InitSocket(uint16_t port);

   class ReceivedPacket {
   public:
      ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);

      const SocketAddress& GetFromAddress() const { return mFromAddress; }

      float GetReceivedTime() const { return mReceivedTime; }

      InputMemoryBitStream& GetPacketBuffer() { return mPacketBuffer; }

   private:
      float mReceivedTime;
      InputMemoryBitStream mPacketBuffer;
      SocketAddress mFromAddress;
   };

   void ReadIncomingPackets(const float dt);

   void UpdateHighestPlayerId(uint32_t inId);
   void EnterPlayingState();

   // these should stay ordered!
   typedef std::map<uint32_t, SocketAddress> IntToSocketAddrMap;
   typedef std::map<uint32_t, std::string> IntToStrMap;
   // typedef std::map<uint32_t, TurnData> IntToTurnDataMap;
   // typedef std::map<uint32_t, GameObjectPtr> IntToGameObjectMap;

   typedef std::unordered_map<SocketAddress, uint32_t> SocketAddrToIntMap;

   // bool CheckSync(IntToTurnDataMap& inTurnMap);

   // IntToGameObjectMap mNetworkIdToGameObjectMap;
   IntToSocketAddrMap m_playerToSocketMap;
   SocketAddrToIntMap m_socketToPlayerMap;
   IntToStrMap m_playerNameMap;

   // this stores all of our turn information for every turn since game start
   // std::vector<IntToTurnDataMap> mTurnData;

   UdpSocketPtr m_socket;
   SocketAddress m_masterPeerAddr;

   NetworkManagerState m_state;

   std::string m_name;

   float m_helloTimeCounter;
   float m_timeToStart;

   int m_playerCount;

   // we track the highest player id seen in the event
   // the master peer d/cs and we need a new master peer
   // who can assign ids
   uint32_t m_highestPlayerId;
   uint32_t m_newNetworkId;
   uint32_t m_playerId;

   int m_turnNumber;
   int m_subTurnNumber;
   bool m_isMasterPeer;
};

} // namespace BIEngine
