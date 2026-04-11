#pragma once

#include "../../BIEngine/Network/Peer.h"
#include "../PlayerManager/PlayerManager.h"

void RpcInit();
void RpcWriteSetPlayer(BIEngine::PeerId peerId, PlayerId id);
void RpcReadSetPlyaer(BIEngine::InputMemoryBitStream& data);