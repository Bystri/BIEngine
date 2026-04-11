#include "RPCManager.h"

#include "../../StdLib/Assert.h"

namespace BIEngine {

void RPCManager::RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func)
{
   Assert(m_nameToRPCTable.Find(id) == m_nameToRPCTable.End(), "RPC function with id %u already registered in RPCManager", id);
   m_nameToRPCTable[id] = func;
}

void RPCManager::ProcessRPC(InputMemoryBitStream& stream)
{
   uint32_t id;
   Deserialize(stream, id);
   m_nameToRPCTable[id](stream);
}

} // namespace BIEngine
