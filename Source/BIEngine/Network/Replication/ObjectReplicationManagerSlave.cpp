#include "ObjectReplicationManagerSlave.h"

namespace BIEngine {

void ObjectReplicationManagerSlave::ProcessPacket(InputMemoryBitStream& stream)
{
   while (stream.GetRemainingBitCount() >= 32) {
      m_pReplicationActionReader->ProcessReplicationAction(stream);
   }
}

} // namespace BIEngine
