#include "RPCManager.h"

#include "../../EngineCore/Assert.h"

namespace BIEngine {

void RPCManager::RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func)
{
   Assert(m_nameToRPCTable.find(id) == m_nameToRPCTable.end(), "RPC function with id %u already registered in RPCManager", id);
   m_nameToRPCTable[id] = func;
}

void RPCManager::ProcessRPC(InputMemoryBitStream& stream)
{
   uint32_t name;
   Deserialize(stream, name);
   m_nameToRPCTable[name](stream);
}

} // namespace BIEngine
