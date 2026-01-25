#include "ReplicationManagerTransmissionData.h"

namespace BIEngine {

void ReplicationManagerTransmissionData::AddTransmission(int networkId, ReplicationAction action, const ReplicationObject::DirtyStateMask& state)
{
   m_transmissions.EmplaceBack(networkId, action, state);
}

void ReplicationManagerTransmissionData::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
   // run through the transmissions
   for (const ReplicationTransmission& rt : m_transmissions) {
      // is it a create? then we have to redo the create.
      int networkId = rt.GetNetworkId();

      switch (rt.GetAction()) {
         case ReplicationAction::Create:
            HandleCreateDeliveryFailure(networkId);
            break;
         case ReplicationAction::Update:
            HandleUpdateStateDeliveryFailure(networkId, rt.GetState(), inDeliveryNotificationManager);
            break;
         case ReplicationAction::Destroy:
            HandleDestroyDeliveryFailure(networkId);
            break;
      }
   }
}

void ReplicationManagerTransmissionData::HandleCreateDeliveryFailure(int networkId) const
{
   // does the object still exist? it might be dead, in which case we don't resend a create
   GameObjectPtr gameObject = NetworkManagerServer::sInstance->GetGameObject(networkId);
   if (gameObject) {
      m_pReplicationActionWriter->ReplicateCreate(networkId);
   }
}

void ReplicationManagerTransmissionData::HandleDestroyDeliveryFailure(int networkId) const
{
   m_pReplicationActionWriter->ReplicateDestroy(networkId);
}

void ReplicationManagerTransmissionData::HandleUpdateStateDeliveryFailure(int networkId, uint32_t state, DeliveryNotificationManager* pDeliveryNotificationManager) const
{
   // does the object still exist? it might be dead, in which case we don't resend an update
   if (NetworkManagerServer::sInstance->GetGameObject(networkId)) {
      // look in all future in flight packets, in all transmissions
      // remove written state from dirty state
      for (const auto& inFlightPacket : pDeliveryNotificationManager->GetInFlightPackets()) {
         ReplicationManagerTransmissionDataPtr rmtdp = StaticPointerCast<ReplicationManagerTransmissionData>(inFlightPacket.GetTransmissionData('RPLM'));

         for (const ReplicationTransmission& otherRT : rmtdp->m_transmissions) {
            if (otherRT.GetNetworkId() == networkId) {
               state &= ~otherRT.GetState();
            }
         }
      }

      // if there's still any dirty state, mark it
      if (state) {
         m_pReplicationActionWriter->SetStateDirty(networkId, state);
      }
   }
}

} // namespace BIEngine
