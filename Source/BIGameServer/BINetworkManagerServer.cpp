#include "BINetworkManagerServer.h"

#include "../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Network/EventNetworkProtocol.h"

#include "BIGSEventListener.h"

bool BINetworkManagerServer::Init(uint16_t port)
{
   m_protocolsManager.AddProtocolWriter(BIEngine::MakeShared<BIEngine::ObjectReplicationProtocolWriter>());
   m_protocolsManager.AddProtocolReader(BIEngine::MakeShared<EventProtocolReader>());
   return NetworkManager::InitInternal(port);
}

void BINetworkManagerServer::ProcessPacket(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   // try to get the client proxy for this address
   // pass this to the client proxy to process
   auto it = m_addressToClientMap.Find(fromAddress);
   if (it == m_addressToClientMap.End()) {
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

   auto infoItr = m_peerInfoMap.Find(clientProxy->GetId());
   BIEngine::Assert(infoItr != m_peerInfoMap.End(), "Try to process packet from an unknown peer");

   if (infoItr == m_peerInfoMap.End()) {
      return;
   }

   infoItr->second.deliveryNotificationManager.ReadAndProcessState(inputStream);

   while (inputStream.GetRemainingBitCount() > 32) {
      MessageToRead& msg = infoItr->second.messageQueueToRead.EmplaceBack();
      msg.Read(inputStream);
   }

   Sort(infoItr->second.messageQueueToRead.Begin(), infoItr->second.messageQueueToRead.End());
}

void BINetworkManagerServer::HandlePacketFromNewClient(BIEngine::InputMemoryBitStream& inputStream, const BIEngine::SocketAddress& fromAddress)
{
   // read the beginning- is it a hello?

   BIEngine::PeerPtr newClient = BIEngine::MakeShared<BIEngine::Peer>(m_nextClientId, fromAddress);
   ++m_nextClientId;

   auto itr = m_peerInfoMap.Emplace(newClient->GetId(), newClient);

   itr.first->second.deliveryNotificationManager.ReadAndProcessState(inputStream);

   MessageToRead msg;
   msg.Read(inputStream);
   BIEngine::Assert(msg.GetId() == 0, "First msg id expected to be 0");

   uint32_t packetType;
   BIEngine::Deserialize(msg.GetBuffer(), packetType);

   if (packetType == kHelloCC) {
      // read the name
      BIEngine::String name;
      BIEngine::Deserialize(msg.GetBuffer(), name);

      m_protocolsManager.RegisterPeer(newClient);

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
   BIEngine::OutputMemoryBitStream welcomeMessage;

   BIEngine::Serialize(welcomeMessage, clientProxy->GetId());

   BIEngine::Logger::WriteMsgLog("Server Welcoming, new client with id %d", clientProxy->GetId());

   SendNetworkMessage(*clientProxy, kWelcomeCC, welcomeMessage);
}

void BINetworkManagerServer::SendOutgoingPackets(const BIEngine::GameTimer& gt)
{
   m_protocolsManager.OnBeforePacketsSend(this);
   SendMessagesFromQueue(gt);
}
