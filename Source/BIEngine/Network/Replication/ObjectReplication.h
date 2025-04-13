#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "ReplicationUnit.h"
#include "../Serialization.h"
#include "../NetworkManager.h"
#include "../../Utilities/Logger.h"

namespace BIEngine {

#define CLASS_IDENTIFICATION(inCode)     \
   enum { sk_ClassType = inCode };       \
   virtual uint32_t GetClassType() const \
   {                                     \
      return sk_ClassType;               \
   }

class ReplicationObject {
public:
   CLASS_IDENTIFICATION('GOBJ')

   virtual ~ReplicationObject() = default;

   virtual void Init(bool isMaster) {}

   virtual bool IsDirty() const { return true; }

   virtual void OnUpdate() {}

   virtual void Write(OutputMemoryBitStream& stream, bool ignoreDirtyFlag) {}

   virtual void Read(InputMemoryBitStream& stream) {}
};

template <class ReplicatedObject>
class ReplicationObjectT : public ReplicationObject {
public:
   using ReplicationUnitArray = std::vector<std::shared_ptr<ReplicationUnit<ReplicatedObject>>>;

   ReplicationObjectT(ReplicationUnitArray replicationUnits)
      : m_isDirtyMask(0u), m_replicationUnits(std::move(replicationUnits))
   {
   }

   void Init(bool isMaster)
   {
      m_pReplicatedObject = ConstructReplicatedObject(isMaster);

      for (int i = 0; i < m_replicationUnits.size(); ++i) {
         m_replicationUnits[i]->Init(m_pReplicatedObject);
      }
   }

   void OnUpdate()
   {
      m_isDirtyMask = 0u;
      for (int i = 0; i < m_replicationUnits.size(); ++i) {
         if (m_replicationUnits[i]->IsStateChanged()) {
            m_isDirtyMask |= 1 << i;
         }
      }
   }

   std::shared_ptr<ReplicatedObject> GetReplicatedObject() const { return m_pReplicatedObject; }

   bool IsDirty() const { return m_isDirtyMask != 0u; }

   void Write(OutputMemoryBitStream& stream, bool ignoreDirtyFlag)
   {
      const uint32_t dirtyMask = ignoreDirtyFlag ? 0xffffffff : m_isDirtyMask;

      for (int i = 0; i < m_replicationUnits.size(); ++i) {
         if ((dirtyMask & 1 << i) == 0) {
            Serialize(stream, false, 1);
            continue;
         }

         Serialize(stream, true, 1);
         m_replicationUnits[i]->Write(stream);
      }
   }

   void Read(InputMemoryBitStream& stream)
   {
      unsigned char dirtyFlag;
      for (int i = 0; i < m_replicationUnits.size(); ++i) {
         Deserialize(stream, dirtyFlag, 1);

         if (dirtyFlag == 0) {
            continue;
         }

         m_replicationUnits[i]->Read(stream);
      }
   }

protected:
   virtual std::shared_ptr<ReplicatedObject> ConstructReplicatedObject(bool isMaster) = 0;

private:
   uint32_t m_isDirtyMask;

   ReplicationUnitArray m_replicationUnits;

   std::shared_ptr<ReplicatedObject> m_pReplicatedObject;
};

} // namespace BIEngine
