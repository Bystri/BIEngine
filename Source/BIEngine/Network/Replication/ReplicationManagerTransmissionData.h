#include "ReplicationHeader.h"
#include "ObjectReplicationManagerMaster.h"
#include "../DeliveryNotificationManager.h"

namespace BIEngine {

class ReplicationManagerTransmissionData : public TransmissionData {
public:
   ReplicationManagerTransmissionData(ReplicationActionWriter* pReplicationActionWriter)
      : m_pReplicationActionWriter(pReplicationActionWriter)
   {
   }

   class ReplicationTransmission {
   public:
      ReplicationTransmission(int networkId, ReplicationAction action, uint32_t state)
         : m_networkId(networkId),
           m_action(action),
           m_state(state)
      {
      }

      int GetNetworkId() const { return m_networkId; }

      ReplicationAction GetAction() const { return m_action; }

      uint32_t GetState() const { return m_state; }

   private:
      int m_networkId;
      ReplicationAction m_action;
      uint32_t m_state;
   };

   void AddTransmission(int networkId, ReplicationAction action, const ReplicationObject::DirtyStateMask& state);

   virtual void HandleDeliveryFailure(DeliveryNotificationManager* pDeliveryNotificationManager) const override;

private:
   void HandleCreateDeliveryFailure(int networkId) const;
   void HandleUpdateStateDeliveryFailure(int networkId, uint32_t state, DeliveryNotificationManager* pDeliveryNotificationManager) const;
   void HandleDestroyDeliveryFailure(int networkId) const;

private:
   ReplicationActionWriter* m_pReplicationActionWriter;

   DynamicArray<ReplicationTransmission> m_transmissions;
};

using ReplicationManagerTransmissionDataPtr = SharedPtr<ReplicationManagerTransmissionData>;

} // namespace BIEngine
