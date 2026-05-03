#include "BINetworkRPCs.h"

#include "../../BIEngine/Network/RpcProtocol.h"

enum class RPCs : BIEngine::RpcId {
   SET_PLAYER_FOR_CLIENT
};

void RpcInit()
{
   BIEngine::RpcProtocolReader::Get()->RegisterUnwrapFunction(static_cast<BIEngine::RpcId>(RPCs::SET_PLAYER_FOR_CLIENT), RpcReadSetPlyaer);
}

void RpcWriteSetPlayer(BIEngine::PeerId peerId, PlayerId id)
{
   BIEngine::OutputMemoryBitStream data;
   BIEngine::Serialize(data, id);
   BIEngine::RpcProtocolWriter::Get()->SendRpc(peerId, static_cast<BIEngine::RpcId>(RPCs::SET_PLAYER_FOR_CLIENT), data);
}

void RpcReadSetPlyaer(BIEngine::InputMemoryBitStream& data)
{
   PlayerId id;
   BIEngine::Deserialize(data, id);
   PlayerManager::Get()->SetLocalPlayer(id);
}