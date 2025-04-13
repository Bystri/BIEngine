#pragma once

#include <unordered_map>

#include "../Serialization.h"

namespace BIEngine {

using RPCUnwrapFunc = void (*)(InputMemoryBitStream&);

class RPCManager {
public:
   void RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func);
   void ProcessRPC(InputMemoryBitStream& stream);

private:
   std::unordered_map<uint32_t, RPCUnwrapFunc> m_nameToRPCTable;
};

} // namespace BIEngine
