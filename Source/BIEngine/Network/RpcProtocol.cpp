#include "RpcProtocol.h"

#include "NetworkMessagesManager.h"

namespace BIEngine {

const NetworkProtocolType RpcProtocolLeader::sk_ProtocolType('RPC0');
const NetworkProtocolType RpcProtocolFollower::sk_ProtocolType('RPC0');

static RpcProtocolLeader* g_pRpcProtocolWriter;

RpcProtocolLeader* RpcProtocolLeader::Get()
{
   Assert(g_pRpcProtocolWriter != nullptr, "You are trying to get RpcProtocolWriter before it was created");

   return g_pRpcProtocolWriter;
}

RpcProtocolLeader::RpcProtocolLeader()
{
   if (g_pRpcProtocolWriter) {
      Logger::WriteErrorLog("Attempting to create two global RpcProtocolWriters managers! The old one will be destroyed and overwritten with this one.\n");
   }

   g_pRpcProtocolWriter = this;
}

RpcProtocolLeader::~RpcProtocolLeader()
{
   if (g_pRpcProtocolWriter == this) {
      g_pRpcProtocolWriter = nullptr;
   }
}

void RpcProtocolLeader::SendRpc(PeerId peerId, RpcId rpcId, const OutputMemoryBitStream& rpcData)
{
   for (int i = 0; i < m_peerInfos.Size(); ++i) {
      if (m_peerInfos[i].id == peerId) {
         OutputMemoryBitStream& data = m_peerInfos[i].m_rpcToSend.EmplaceBack();
         Serialize(data, rpcId);
         data.WriteBits(rpcData.GetBufferPtr().Get(), rpcData.GetBitLength());
         return;
      }
   }

   Assert(false, "You are trying to send RPC to unregistered peer [id:%u]", peerId);
}

void RpcProtocolLeader::RegisterPeer(PeerId peerId)
{
   PeerInfo& info = m_peerInfos.EmplaceBack();
   info.id = peerId;
}

void RpcProtocolLeader::UnregisterPeer(PeerId peerId)
{
   for (int i = 0; i < m_peerInfos.Size(); ++i) {
      if (m_peerInfos[i].id == peerId) {
         m_peerInfos.Erase(m_peerInfos.Begin() + i);
         return;
      }
   }

   Assert(false, "You are trying to delete unregistered peer [id:%u] from RpcProtocolWriter", peerId);
}

void RpcProtocolLeader::OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager)
{
   for (auto& pPeer : m_peerInfos) {
      if (pPeer.m_rpcToSend.Empty()) {
         continue;
      }

      OutputMemoryBitStream packet;
      const uint32_t cnt = pPeer.m_rpcToSend.Size();
      Serialize(packet, cnt);

      for (int i = 0; i < pPeer.m_rpcToSend.Size(); ++i)
      {
         packet.WriteBits(pPeer.m_rpcToSend[i].GetBufferPtr().Get(), pPeer.m_rpcToSend[i].GetBitLength());
      }

      pPeer.m_rpcToSend.Clear();

      pNetworkMessagesManager->SendNetworkMessage(pPeer.id, GetType(), packet);
   }
}

static RpcProtocolFollower* g_pRpcProtocolReader;

RpcProtocolFollower* RpcProtocolFollower::Get()
{
   Assert(g_pRpcProtocolReader != nullptr, "You are trying to get RpcProtocolWriter before it was created");

   return g_pRpcProtocolReader;
}

RpcProtocolFollower::RpcProtocolFollower()
{
   if (g_pRpcProtocolReader) {
      Logger::WriteErrorLog("Attempting to create two global RpcProtocolReader managers! The old one will be destroyed and overwritten with this one.\n");
   }

   g_pRpcProtocolReader = this;
}

RpcProtocolFollower::~RpcProtocolFollower()
{
   if (g_pRpcProtocolReader == this) {
      g_pRpcProtocolReader = nullptr;
   }
}

void RpcProtocolFollower::RegisterUnwrapFunction(RpcId id, RPCUnwrapFunc func)
{
   Assert(m_nameToRPCTable.Find(id) == m_nameToRPCTable.End(), "RPC function with id %u already registered in RPCManager", id);
   m_nameToRPCTable[id] = func;
}

void RpcProtocolFollower::ProcessRPC(InputMemoryBitStream& stream)
{
   RpcId id;
   Deserialize(stream, id);
   m_nameToRPCTable[id](stream);
}

void RpcProtocolFollower::ReceiveMessage(PeerId peerId, BIEngine::InputMemoryBitStream& inputStream)
{
   uint32_t cnt;
   Deserialize(inputStream, cnt);

   for (int i = 0; i < cnt; ++i)
   {
      ProcessRPC(inputStream);
   }
}

} // namespace BIEngine