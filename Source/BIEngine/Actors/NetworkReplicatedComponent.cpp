#include "NetworkReplicatedComponent.h"

#include "../Network/Replication/ObjectReplicationProtocol.h"

namespace BIEngine {

const ComponentId NetworkReplicatedComponent::g_CompId = "NetworkReplicatedComponent";

bool NetworkReplicatedComponent::Init(tinyxml2::XMLElement* pData)
{
   return true;
}

tinyxml2::XMLElement* NetworkReplicatedComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());

   return pBaseElement;
}

void NetworkReplicatedComponent::Terminate()
{
   ObjectReplicationDestroy(m_pReplicationObject);
}

} // namespace BIEngine