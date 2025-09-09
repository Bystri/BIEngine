#include "CombatStateComponent.h"

#include "../../BIEngine/StdLib/Assert.h"

const BIEngine::ComponentId CombatStateComponent::g_CompId = "CombatStateComponent";

bool CombatStateComponent::Init(tinyxml2::XMLElement* pData)
{
   BIEngine::Assert(pData, "Bad arguments provided for initialization to Locomotion Info Component");
   return true;
}

tinyxml2::XMLElement* CombatStateComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().CStr());
   return pBaseElement;
}
