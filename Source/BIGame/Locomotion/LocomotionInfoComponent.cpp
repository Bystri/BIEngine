#include "LocomotionInfoComponent.h"

#include "../../BIEngine/EngineCore/Assert.h"

const BIEngine::ComponentId LocomotionInfoComponent::g_CompId = "LocomotionInfoComponent";

bool LocomotionInfoComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to Locomotion Info Component");
   return true;
}

tinyxml2::XMLElement* LocomotionInfoComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());
   return pBaseElement;
}
