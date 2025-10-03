#include "HealthStateComponent.h"

#include "../../BIEngine/StdLib/Assert.h"

const BIEngine::ComponentId HealthStateComponent::g_CompId = "HealthStateComponent";

bool HealthStateComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to HealthStateComponent");
   return true;
}

tinyxml2::XMLElement* HealthStateComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}
