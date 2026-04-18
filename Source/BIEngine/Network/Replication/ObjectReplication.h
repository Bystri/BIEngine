#pragma once

#include <memory>

#include "ReplicationUnit.h"
#include "../Serialization.h"
#include "../NetworkManager.h"
#include "../../StdLib/Bitset.h"
#include "../../Utilities/Logger.h"
#include "../../EngineCore/GameApp.h"

namespace BIEngine {

#define CLASS_IDENTIFICATION(inCode)     \
   enum { sk_ClassType = inCode };       \
   virtual uint32_t GetClassType() const \
   {                                     \
      return sk_ClassType;               \
   }

class ReplicationObject : public EnableSharedFromThis<ReplicationObject> {
public:
   CLASS_IDENTIFICATION('GOBJ')

   virtual ~ReplicationObject() = default;

   virtual void Init(PeerId masterPeerId)
   {
      m_masterPeerId = masterPeerId;
   }

   virtual void Term() {}

   PeerId GetMasterPeerId() const
   {
      return m_masterPeerId;
   }

   virtual bool IsDirty() const { return true; }

   virtual void OnUpdate() {}

   virtual void Write(OutputMemoryBitStream& stream, bool ignoreDirtyFlag) {}

   virtual void Read(InputMemoryBitStream& stream) {}

private:
   PeerId m_masterPeerId = -1;
};

template <class ReplicatedObject>
class ReplicationObjectT : public ReplicationObject {
public:
   using ReplicationUnitArray = DynamicArray<SharedPtr<ReplicationUnit<ReplicatedObject>>>;

   ReplicationObjectT(ReplicationUnitArray replicationUnits)
      : m_isDirtyMask(), m_replicationUnits(std::move(replicationUnits))
   {
   }

   virtual void Init(PeerId masterPeerId) override
   {
      ReplicationObject::Init(masterPeerId);

      m_pReplicatedObject = ConstructReplicatedObject(masterPeerId == g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId());

      for (int i = 0; i < m_replicationUnits.Size(); ++i) {
         m_replicationUnits[i]->Init(this, m_pReplicatedObject);
      }
   }

   virtual void Term() override
   {
      ReplicationObject::Term();

      DestructReplicationObject(GetMasterPeerId() == g_pApp->m_pGameLogic->GetNetworkManager()->GetPeerId());
   }

   virtual void OnUpdate() override
   {
      m_isDirtyMask.Reset();
      for (int i = 0; i < m_replicationUnits.Size(); ++i) {
         if (m_replicationUnits[i]->IsStateChanged()) {
            m_isDirtyMask.Set(i, true);
         }
      }
   }

   SharedPtr<ReplicatedObject> GetReplicatedObject() const { return m_pReplicatedObject; }

   virtual bool IsDirty() const override { return m_isDirtyMask.Any(); }

   virtual void Write(OutputMemoryBitStream& stream, bool ignoreDirtyFlag) override
   {
      for (int i = 0; i < m_replicationUnits.Size(); ++i) {
         if (ignoreDirtyFlag || m_isDirtyMask[i]) {
            Serialize(stream, true, 1);
            m_replicationUnits[i]->Write(stream);
            continue;
         }

         Serialize(stream, false, 1);
      }
   }

   virtual void Read(InputMemoryBitStream& stream) override
   {
      unsigned char dirtyFlag;
      for (int i = 0; i < m_replicationUnits.Size(); ++i) {
         Deserialize(stream, dirtyFlag, 1);

         if (dirtyFlag == 0) {
            continue;
         }

         m_replicationUnits[i]->Read(stream);
      }
   }

protected:
   virtual SharedPtr<ReplicatedObject> ConstructReplicatedObject(bool isMaster) = 0;
   virtual void DestructReplicationObject(bool isMaster) = 0;

private:
   Bitset<32> m_isDirtyMask;

   ReplicationUnitArray m_replicationUnits;

   SharedPtr<ReplicatedObject> m_pReplicatedObject;
};

} // namespace BIEngine
