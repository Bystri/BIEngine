#include "BINetworkManagerServer.h"

#include "../BIEngine/Network/Replication/ObjectReplicationManagerMaster.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"

#include "BIGSEventListener.h"

bool BINetworkManagerServer::Init(uint16_t port)
{
   return NetworkManager::InitInternal(port);
}

void BINetworkManagerServer::ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   // try to get the client proxy for this address
   // pass this to the client proxy to process
   auto it = m_addressToClientMap.find(fromAddress);
   if (it == m_addressToClientMap.end()) {
      // didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
      HandlePacketFromNewClient(inputStream, fromAddress);
   } else {
      ProcessPacket((*it).second, inputStream);
   }
}

void BINetworkManagerServer::ProcessPacket(BIEngine::PeerPtr clientProxy, BIEngine::InputMemoryBitStream& inputStream)
{
   // remember we got a packet so we know not to disconnect for a bit
   // inClientProxy->UpdateLastPacketTime();

   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);
   switch (packetType) {
      case kHelloCC:
         // need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
         // otherwise something weird is going on...
         SendWelcomePacket(clientProxy);
         break;
      case kEventCC:
         HandleEventPacket(clientProxy, inputStream);
         break;
      default:
         BIEngine::Logger::WriteErrorLog("Unknown packet type received");
         break;
   }
}

void BINetworkManagerServer::HandlePacketFromNewClient(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   // read the beginning- is it a hello?
   uint32_t packetType;
   BIEngine::Deserialize(inputStream, packetType);

   if (packetType == kHelloCC) {
      // read the name
      std::string name;
      BIEngine::Deserialize(inputStream, name);

      BIEngine::PeerPtr newClient = std::make_shared<BIEngine::Peer>(nextClientId, fromAddress);
      ++nextClientId;

      BIEngine::ObjectReplicationManagerMaster::Get()->AddClient(newClient);
      BIEngine::ObjectReplicationCreate(ReplicationObjectPlayer::sk_ClassType);

      m_addressToClientMap[fromAddress] = newClient;

      // and welcome the client...
      SendWelcomePacket(newClient);

   } else {
      // bad incoming packet from unknown client- we're under attack!!
      BIEngine::Logger::WriteErrorLog("Bad incoming packet from unknown client");
   }
}

void BINetworkManagerServer::SendWelcomePacket(BIEngine::PeerPtr clientProxy)
{
   BIEngine::OutputMemoryBitStream welcomePacket;

   BIEngine::Serialize(welcomePacket, kWelcomeCC);
   BIEngine::Serialize(welcomePacket, clientProxy->GetId());

   BIEngine::Logger::WriteMsgLog("Server Welcoming, new client with id %d", clientProxy->GetId());

   SendPacket(welcomePacket, clientProxy->GetSocketAddress());
}

void BINetworkManagerServer::SendOutgoingPackets()
{
   BIEngine::ObjectReplicationManagerMaster::Get()->SendPacket(this);
}

void BINetworkManagerServer::HandleEventPacket(BIEngine::PeerPtr pClientProxy, BIEngine::InputMemoryBitStream& inputStream)
{
   uint8_t eventCount = 0;
   BIEngine::Deserialize(inputStream, eventCount);

   while (eventCount > 0) {
      BIEngine::EventType eventType;
      BIEngine::Deserialize(inputStream, eventType);

      BIEngine::IEventDataPtr pEvent = BIEngine::g_eventFactory.Create(eventType);
      pEvent->Read(inputStream);

      BIEngine::EventManager::Get()->QueueEvent(pEvent);

      --eventCount;
   }
}
