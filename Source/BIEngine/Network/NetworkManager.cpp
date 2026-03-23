#include "NetworkManager.h"

#include <imgui.h>

#include "../Utilities/Logger.h"

namespace BIEngine {

bool NetworkManager::InitInternal(uint16_t port)
{
   return true;
}

void NetworkManager::ProcessIncomingPackets()
{
   m_networkMessagesManager.ProcessMessages();
}

void NetworkManager::SendNetworkMessage(uint32_t peedId, NetworkProtocolType protocolType, const OutputMemoryBitStream& outputStream)
{
   m_networkMessagesManager.SendNetworkMessage(peedId, protocolType, outputStream);
}

void NetworkManager::SendMessagesFromQueue(const GameTimer& gt)
{
   m_networkMessagesManager.SendOutgoingPackets(gt);

}

#ifndef _RETAIL
void NetworkManager::DrawDbgDiagnostics()
{
   m_networkMessagesManager.DrawDbgDiagnostics();
}
#endif

} // namespace BIEngine
