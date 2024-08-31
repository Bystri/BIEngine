#include "NetworkManagerP2P.h"

#include <algorithm>

namespace BIEngine {

namespace {
const float TIME_BETWEEN_HELLOS = 1.f;
const float START_DELAY = 3.0f;
const int SUB_TURNS_PER_TURN = 3;
const int kMaxPlayerCount = 4;
} // namespace

SocketAddress DeserializeAddress(InputMemoryBitStream& inputStream)
{
   // read the address and port
   uint32_t addr;
   uint16_t port;
   Deserialize(inputStream, addr);
   Deserialize(inputStream, port);

   return SocketAddress(addr, port);
}

void SerializeAddress(const SocketAddress& addr, OutputMemoryBitStream& outputStream)
{
   // write the address and port
   Serialize(outputStream, addr.GetAddress());
   Serialize(outputStream, addr.GetPort());
}

std::unique_ptr<NetworkManager> NetworkManager::CreateAsMasterPeer(uint16_t port, const std::string& name)
{
   std::unique_ptr<NetworkManager> manager = std::unique_ptr<NetworkManager>(new NetworkManager);

   if (!manager->InitAsMasterPeer(port, name)) {
      return nullptr;
   }

   return manager;
}

std::unique_ptr<NetworkManager> NetworkManager::CreateAsPeer(const SocketAddress& MPAddress, const std::string& name)
{
   std::unique_ptr<NetworkManager> manager = std::unique_ptr<NetworkManager>(new NetworkManager);

   if (!manager->InitAsPeer(MPAddress, name)) {
      return nullptr;
   }

   return manager;
}

NetworkManager::NetworkManager()
   : m_playerId(0),
     m_newNetworkId(1),
     m_isMasterPeer(false),
     m_state(NetworkManagerState::Unitialized),
     m_playerCount(0),
     m_highestPlayerId(0),
     m_helloTimeCounter(0.0f),
     m_timeToStart(-1.0f)
// we always start on turn -2 b/c we need 2 frames before we can actually play
// mTurnNumber(-2),
// mSubTurnNumber(0)
{
   // this is enough for a 166 minute game...
   // so let's avoid realloc/copies and just construct all the empty maps, too
   // mTurnData.resize(100000);
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::InitAsMasterPeer(uint16_t port, const std::string& name)
{
   if (!InitSocket(port)) {
      return false;
   }

   // master peer at init means guaranteed first player id
   m_playerId = 1;
   m_highestPlayerId = m_playerId;
   m_isMasterPeer = true;
   m_playerCount = 1;

   // in lobby cause we don't need to ask the master peer (since we are the master peer)
   m_state = NetworkManagerState::Lobby;

   // add myself to the player name map
   m_name = name;
   m_playerNameMap.emplace(m_playerId, m_name);
   return true;
}

bool NetworkManager::InitAsPeer(const SocketAddress& MPAddress, const std::string& name)
{
   if (!InitSocket(0)) {
      return false;
   }

   m_masterPeerAddr = MPAddress;

   // we're going to have to ask the master peer
   m_state = NetworkManagerState::Hello;

   // set my name
   m_name = name;
   // don't know my player id, so can't add myself to the name map yet

   return true;
}

bool NetworkManager::InitSocket(uint16_t port)
{
   m_socket = SocketUtil::CreateUdpSocket(SocketAddressFamily::INET);
   SocketAddress ownAddress(INADDR_ANY, port);
   m_socket->Bind(ownAddress);

   Logger::WriteMsgLog("Initializing NetworkManager at port %d", port);

   // did we bind okay?
   if (m_socket == nullptr) {
      return false;
   }

   if (m_socket->SetNonBlockingMode(true) != NO_ERROR) {
      return false;
   }

   return true;
}

void NetworkManager::ProcessIncomingPackets(const float dt)
{
   ReadIncomingPackets(dt);
}

void NetworkManager::SendOutgoingPackets(const GameTimer& gt)
{
   switch (m_state) {
      case NetworkManagerState::Hello:
         UpdateSayingHello(gt.DeltaTime());
         break;
      case NetworkManagerState::Starting:
         UpdateStarting(gt.DeltaTime());
         break;
      case NetworkManagerState::Playing:
         UpdateSendTurnPacket();
         break;
      default:
         break;
   }
}

void NetworkManager::UpdateSayingHello(const float dt)
{
   m_helloTimeCounter += dt;

   if (m_helloTimeCounter >= TIME_BETWEEN_HELLOS) {
      SendHelloPacket();
      m_helloTimeCounter = 0.0f;
   }
}

void NetworkManager::SendHelloPacket()
{
   OutputMemoryBitStream helloPacket;

   Serialize(helloPacket, HELLO_CC);
   Serialize(helloPacket, m_name);

   SendPacket(helloPacket, m_masterPeerAddr);
}

void NetworkManager::UpdateStarting(const float dt)
{
   m_timeToStart -= dt;
   if (m_timeToStart <= 0.0f) {
      EnterPlayingState();
   }
}

void NetworkManager::UpdateSendTurnPacket()
{
   /*
   m_subTurnNumber++;
   if (m_subTurnNumber == SUB_TURNS_PER_TURN) {
      // create our turn data
      TurnData data(m_playerId, RandGen::sInstance->GetRandomUInt32(0, UINT32_MAX), ComputeGlobalCRC(), InputManager::sInstance->GetCommandList());

      // we need to send a turn packet to all of our peers
      OutputMemoryBitStream packet;
      packet.Serialize(TURN_CC);
      // we're sending data for 2 turns from now
      packet.Serialize(mTurnNumber + 2);
      packet.Serialize(mPlayerId);
      data.Serialize(packet);

      for (auto& iter : mPlayerToSocketMap) {
         SendPacket(packet, iter.second);
      }

      // save our turn data for turn + 2
      mTurnData[mTurnNumber + 2].emplace(mPlayerId, data);
      InputManager::sInstance->ClearCommandList();

      if (mTurnNumber >= 0) {
         TryAdvanceTurn();
      } else {
         // a negative turn means there's no possible commands yet
         mTurnNumber++;
         mSubTurnNumber = 0;
      }
   }
   */
}

void NetworkManager::TryAdvanceTurn()
{
   /*
   // only advance the turn IF we received the data for everyone
   if (mTurnData[mTurnNumber + 1].size() == mPlayerCount) {
      if (mState == NMS_Delay) {
         // throw away any input accrued during delay
         InputManager::sInstance->ClearCommandList();
         mState = NMS_Playing;
         // wait 100ms to give the slow peer a chance to catch up
         SDL_Delay(100);
      }

      mTurnNumber++;
      mSubTurnNumber = 0;

      if (CheckSync(mTurnData[mTurnNumber])) {
         // process all the moves for this turn
         for (auto& iter : mTurnData[mTurnNumber]) {
            iter.second.GetCommandList().ProcessCommands(iter.first);
         }
      } else {
         // for simplicity, just kill the game if it desyncs
         LOG("DESYNC: Game over man, game over.");
         Engine::sInstance->SetShouldKeepRunning(false);
      }
   } else {
      // don't have all player's turn data, we have to delay :(
      mState = NMS_Delay;
      LOG("Going into delay state, don't have all the info for turn %d", mTurnNumber + 1);
   }
   */
}

void NetworkManager::ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt)
{
   switch (m_state) {
      case NetworkManagerState::Hello:
         ProcessPacketsHello(inputStream, fromAddress);
         break;
      case NetworkManagerState::Lobby:
         ProcessPacketsLobby(inputStream, fromAddress, dt);
         break;
      case NetworkManagerState::Playing:
         ProcessPacketsPlaying(inputStream, fromAddress);
         break;
      case NetworkManagerState::Delay:
         ProcessPacketsDelay(inputStream, fromAddress);
         break;
      default:
         break;
   }
}

void NetworkManager::ProcessPacketsHello(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // only time we're in hello state is when we are not the master peer and trying to join
   uint32_t packetType;
   Deserialize(inputStream, packetType);

   switch (packetType) {
      case NOT_MASTER_PEER:
         HandleNotMPPacket(inputStream);
         break;
      case WELCOME_CC:
         HandleWelcomePacket(inputStream);
         break;
      case NOT_JOINABLE_CC:
      default:
         // couldn't join or something crazy happened, goodbye cruel world
         Logger::WriteErrorLog("Failed to join game, exiting...");
         break;
   }
}

void NetworkManager::HandleNotMPPacket(InputMemoryBitStream& inputStream)
{
   // this will have the correct master peer address
   m_masterPeerAddr = DeserializeAddress(inputStream);
   // just force send this immediately
   UpdateSayingHello(true);
}

void NetworkManager::HandleWelcomePacket(InputMemoryBitStream& inputStream)
{
   // first is my player id
   int playerId;
   Deserialize(inputStream, playerId);
   UpdateHighestPlayerId(playerId);
   m_playerId = playerId;

   // add me to the name map
   m_playerNameMap.emplace(m_playerId, m_name);

   // now the player id for the master peer
   // add entries for the master peer
   Deserialize(inputStream, playerId);
   UpdateHighestPlayerId(playerId);
   m_playerToSocketMap.emplace(playerId, m_masterPeerAddr);
   m_socketToPlayerMap.emplace(m_masterPeerAddr, playerId);

   // now remaining players
   Deserialize(inputStream, m_playerCount);

   // there's actually count - 1 entries because the master peer won't have an entry for themselves
   for (int i = 0; i < m_playerCount - 1; ++i) {
      Deserialize(inputStream, playerId);
      UpdateHighestPlayerId(playerId);

      SocketAddress socketAddr = DeserializeAddress(inputStream);

      m_playerToSocketMap.emplace(playerId, socketAddr);
      m_socketToPlayerMap.emplace(socketAddr, playerId);
   }

   // now player names
   std::string name;
   for (int i = 0; i < m_playerCount; ++i) {
      Deserialize(inputStream, playerId);
      Deserialize(inputStream, name);
      m_playerNameMap.emplace(playerId, name);
   }

   // now add 1 to the player count cause I've joined
   //(the master peer sends me the old list before adding me)
   m_playerCount++;

   // now we need to send out an intro packet to every peer in the socket map
   OutputMemoryBitStream outPacket;
   Serialize(outPacket, INTRO_CC);
   Serialize(outPacket, m_playerId);
   Serialize(outPacket, m_name);
   for (auto& iter : m_playerToSocketMap) {
      SendPacket(outPacket, iter.second);
   }

   Logger::WriteMsgLog("Player was welcomed from host peer with name %s", m_playerNameMap[m_socketToPlayerMap[m_masterPeerAddr]]);

   // I've been welcomed, so I'm in the lobby now
   m_state = NetworkManagerState::Lobby;
}

void NetworkManager::ProcessPacketsLobby(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt)
{
   // could be someone saying hello, an introduction, or a start packet
   uint32_t packetType;
   Deserialize(inputStream, packetType);

   switch (packetType) {
      case HELLO_CC:
         HandleHelloPacket(inputStream, fromAddress);
         break;
      case INTRO_CC:
         HandleIntroPacket(inputStream, fromAddress);
         break;
      case START_CC:
         HandleStartPacket(inputStream, fromAddress, dt);
         break;
      default:
         Logger::WriteErrorLog("Unexpected packet received in Lobby state. Ignoring.");
         break;
   }
}

void NetworkManager::HandleHelloPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // for now, if I already know of this player, throw away the packet
   // this doesn't work if there's packet loss
   if (m_socketToPlayerMap.find(fromAddress) != m_socketToPlayerMap.end()) {
      return;
   }

   if (m_playerCount >= kMaxPlayerCount) {
      // sorry, can't join if full
      OutputMemoryBitStream outPacket;
      Serialize(outPacket, NOT_JOINABLE_CC);
      SendPacket(outPacket, fromAddress);
      return;
   }

   if (m_isMasterPeer) {
      // it'll only contain the new player's name
      std::string name;
      Deserialize(inputStream, name);

      OutputMemoryBitStream outputStream;
      Serialize(outputStream, WELCOME_CC);
      // we'll assign the next possible player id to this player
      m_highestPlayerId++;
      Serialize(outputStream, m_highestPlayerId);

      // write our player id
      Serialize(outputStream, m_playerId);

      Serialize(outputStream, m_playerCount);

      // now write the player to socket map
      for (auto& iter : m_playerToSocketMap) {
         Serialize(outputStream, iter.first);
         SerializeAddress(iter.second, outputStream);
      }

      // and the player names
      for (auto& iter : m_playerNameMap) {
         Serialize(outputStream, iter.first);
         Serialize(outputStream, iter.second);
      }

      SendPacket(outputStream, fromAddress);

      // increment the player count and add this player to maps
      m_playerCount++;
      m_playerToSocketMap.emplace(m_highestPlayerId, fromAddress);
      m_socketToPlayerMap.emplace(fromAddress, m_highestPlayerId);
      m_playerNameMap.emplace(m_highestPlayerId, name);

      Logger::WriteMsgLog("New player with name %s send hello packet", name);
   } else {
      // talk to the master peer, not me
      OutputMemoryBitStream outPacket;
      Serialize(outPacket, NOT_MASTER_PEER);
      SerializeAddress(m_masterPeerAddr, outPacket);
      SendPacket(outPacket, fromAddress);
   }
}

void NetworkManager::HandleIntroPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // master peer can safely ignore the intro packet
   if (!m_isMasterPeer) {
      // just contains player's id and name
      int playerId;
      std::string name;

      Deserialize(inputStream, playerId);
      Deserialize(inputStream, name);

      UpdateHighestPlayerId(playerId);

      m_playerCount++;

      // add the new player to our maps
      m_playerToSocketMap.emplace(playerId, fromAddress);
      m_socketToPlayerMap.emplace(fromAddress, playerId);
      m_playerNameMap.emplace(playerId, name);
   }
}

void NetworkManager::HandleStartPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress, const float dt)
{
   // make sure this is the master peer, cause we don't want any funny business
   if (fromAddress == m_masterPeerAddr) {
      Logger::WriteMsgLog("Got the orders to go!");
      // get the rng seed
      uint32_t seed = 1;
      Deserialize(inputStream, seed);
      // RandGen::sInstance->Seed(seed);
      //  for now, assume that we're one frame off, but ideally we would RTT to adjust
      //  the time to start, based on latency/jitter
      m_state = NetworkManagerState::Starting;
      m_timeToStart = START_DELAY - dt;
   }
}

void NetworkManager::ProcessPacketsPlaying(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   uint32_t packetType;
   Deserialize(inputStream, packetType);

   switch (packetType) {
      case TURN_CC:
         HandleTurnPacket(inputStream, fromAddress);
         break;
      default:
         // ignore anything else
         break;
   }
}

void NetworkManager::HandleTurnPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   /*
   if (mSocketToPlayerMap.find(fromAddress) != mSocketToPlayerMap.end()) {
      int expectedId = mSocketToPlayerMap[fromAddress];

      int turnNum;
      int playerId;
      Deserialize(inputStream, turnNum);
      Deserialize(inputStream, playerId);

      if (playerId != expectedId) {
         LOG("We received turn data for a different player Id...stop trying to cheat!");
         return;
      }

      TurnData data;
      data.Deserialize(inputStream);

      mTurnData[turnNum].emplace(playerId, data);
   }
   */
}

void NetworkManager::ProcessPacketsDelay(InputMemoryBitStream& inputStream, const SocketAddress& fromAddress)
{
   // the only packet we can even consider here is an input one, since we
   // only can only enter delay after we've been playing
   uint32_t packetType;
   Deserialize(inputStream, packetType);

   if (packetType == TURN_CC) {
      HandleTurnPacket(inputStream, fromAddress);
      // if we're lucky, maybe this was the packet we were waiting on?
      TryAdvanceTurn();
   }
}

void NetworkManager::HandleConnectionReset(const SocketAddress& fromAddress)
{
   // remove this player from our maps
   if (m_socketToPlayerMap.find(fromAddress) != m_socketToPlayerMap.end()) {
      uint32_t playerId = m_socketToPlayerMap[fromAddress];
      m_socketToPlayerMap.erase(fromAddress);
      m_playerToSocketMap.erase(playerId);
      m_playerNameMap.erase(playerId);

      m_playerCount--;

      // if this was the master peer, pick the next player in the string map to be MP
      if (fromAddress == m_masterPeerAddr) {
         uint32_t newMPId = m_playerNameMap.begin()->first;
         if (newMPId == m_playerId) {
            // I'm the new master peer, muahahahah
            m_isMasterPeer = true;
         } else {
            m_masterPeerAddr = m_playerToSocketMap[newMPId];
         }
      }

      // if we were in delay, then let's see if we can continue now that this player DC'd?
      if (m_state == NetworkManagerState::Delay) {
         TryAdvanceTurn();
      }
   }
}

void NetworkManager::ReadIncomingPackets(const float dt)
{
   constexpr int packetSize = 1500;
   std::shared_ptr<char> segment(static_cast<char*>(std::malloc(packetSize)), std::free);
   InputMemoryBitStream inputStream(segment, packetSize * 8);
   SocketAddress fromAddress;

   // keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
   int receivedPackedCount = 0;
   int totalReadByteCount = 0;

   while (receivedPackedCount < MAX_PACKETS_PER_FRAME_COUNT) {
      int readByteCount = m_socket->ReceiveFrom(segment.get(), packetSize, fromAddress);
      if (readByteCount == 0) {
         // nothing to read
         break;
      } else if (readByteCount == -WSAECONNRESET) {
         // port closed on other end, so DC this person immediately
         HandleConnectionReset(fromAddress);
      } else if (readByteCount > 0) {
         inputStream.ResetToCapacity(readByteCount);
         ++receivedPackedCount;
         totalReadByteCount += readByteCount;

         ProcessPacket(inputStream, fromAddress, dt);
      } else {
         Logger::WriteErrorLog("Something went wrong in NetworkManager::ReadIncomingPacketsIntoQueue");
      }
   }
}

void NetworkManager::UpdateHighestPlayerId(uint32_t inId)
{
   m_highestPlayerId = max(m_highestPlayerId, inId);
}

void NetworkManager::EnterPlayingState()
{
   m_state = NetworkManagerState::Playing;
}

/*
bool NetworkManager::CheckSync(IntToTurnDataMap& inTurnMap)
{
   auto iter = inTurnMap.begin();

   uint32_t expectedRand = iter->second.GetRandomValue();
   uint32_t expectedCRC = iter->second.GetCRC();

   ++iter;
   while (iter != inTurnMap.end()) {
      if (expectedRand != iter->second.GetRandomValue()) {
         LOG("Random is out of sync for player %u on turn %d", iter->second.GetPlayerId(), mTurnNumber);
         return false;
      }

      if (expectedCRC != iter->second.GetCRC()) {
         LOG("CRC is out of sync for player %u on turn %d", iter->second.GetPlayerId(), mTurnNumber);
         return false;
      }
      ++iter;
   }

   return true;
}
*/

void NetworkManager::SendPacket(const OutputMemoryBitStream& outputStream, const SocketAddress& toAddress)
{
   m_socket->SendTo(outputStream.GetBufferPtr().get(), outputStream.GetByteLength(), toAddress);
}

void NetworkManager::TryStartGame()
{
   if (m_state < NetworkManagerState::Starting && IsMasterPeer()) {
      Logger::WriteMsgLog("Master peer starting the game!");

      // let everyone know
      OutputMemoryBitStream outPacket;
      Serialize(outPacket, START_CC);

      // select a seed value
      uint32_t seed = 1;
      // uint32_t seed = RandGen::sInstance->GetRandomUInt32(0, UINT32_MAX);
      // RandGen::sInstance->Seed(seed);
      Serialize(outPacket, seed);

      for (auto& iter : m_playerToSocketMap) {
         SendPacket(outPacket, iter.second);
      }

      m_timeToStart = START_DELAY;
      m_state = NetworkManagerState::Starting;
   }
}

NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& fromAddress)
   : mReceivedTime(inReceivedTime),
     mFromAddress(fromAddress),
     mPacketBuffer(ioInputMemoryBitStream)
{
}

/*
GameObjectPtr NetworkManager::GetGameObject(uint32_t inNetworkId) const
{
   auto gameObjectIt = mNetworkIdToGameObjectMap.find(inNetworkId);
   if (gameObjectIt != mNetworkIdToGameObjectMap.end()) {
      return gameObjectIt->second;
   } else {
      return GameObjectPtr();
   }
}

GameObjectPtr NetworkManager::RegisterAndReturn(GameObject* inGameObject)
{
   GameObjectPtr toRet(inGameObject);
   RegisterGameObject(toRet);
   return toRet;
}

void NetworkManager::UnregisterGameObject(GameObject* inGameObject)
{
   int networkId = inGameObject->GetNetworkId();
   auto iter = mNetworkIdToGameObjectMap.find(networkId);
   if (iter != mNetworkIdToGameObjectMap.end()) {
      mNetworkIdToGameObjectMap.erase(iter);
   }
}

void NetworkManager::AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
   mNetworkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
   mNetworkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
}


void NetworkManager::RegisterGameObject(GameObjectPtr inGameObject)
{
   // assign network id
   int newNetworkId = GetNewNetworkId();
   inGameObject->SetNetworkId(newNetworkId);

   // add mapping from network id to game object
   mNetworkIdToGameObjectMap[newNetworkId] = inGameObject;
}
*/

uint32_t NetworkManager::GetNewNetworkId()
{
   uint32_t toRet = m_newNetworkId++;
   if (m_newNetworkId < toRet) {
      Logger::WriteErrorLog("Network ID Wrap Around!!! You've been playing way too long...");
   }

   return toRet;
}

/*
uint32_t NetworkManager::ComputeGlobalCRC()
{
   // save into bit stream to reduce CRC calls
   OutputMemoryBitStream crcStream;

   uint32_t crc = static_cast<uint32_t>(crc32(0, Z_NULL, 0));

   for (auto& iter : mNetworkIdToGameObjectMap) {
      iter.second->WriteForCRC(crcStream);
   }

   crc = static_cast<uint32_t>(
      crc32(crc, reinterpret_cast<const Bytef*>(crcStream.GetBufferPtr()), crcStream.GetByteLength()));
   return crc;
}
*/
} // namespace BIEngine
