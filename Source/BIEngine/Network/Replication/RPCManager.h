#pragma once

#include "../Serialization.h"
#include "../../StdLib/HashMap.h"

namespace BIEngine {

using RPCUnwrapFunc = void (*)(InputMemoryBitStream&);

class RPCManager {
public:
   void RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func);
   void ProcessRPC(InputMemoryBitStream& stream);

private:
   HashMap<uint32_t, RPCUnwrapFunc> m_nameToRPCTable;
};

} // namespace BIEngine
