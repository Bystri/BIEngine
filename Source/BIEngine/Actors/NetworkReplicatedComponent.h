#pragma once

#include "../Network/Replication/ReplicationObjectActor/ReplicationObjectActor.h"
#include "ActorComponent.h"

namespace BIEngine {

#pragma optimize("",off)

class NetworkReplicatedComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return NetworkReplicatedComponent::g_CompId; };

public:
   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual void Terminate() override;

   void SetReplicationObject(SharedPtr<ReplicationObject> pReplicationObject) { 
       m_pReplicationObject = pReplicationObject; 
   }

private:
   SharedPtr<ReplicationObject> m_pReplicationObject;
};

static UniquePtr<ActorComponent> CreateNetworkReplicationComponent()
{
   return MakeUnique<NetworkReplicatedComponent>();
}

} // namespace BIEngine
