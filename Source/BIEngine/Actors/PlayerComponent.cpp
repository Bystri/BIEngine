#include "PlayerComponent.h"

namespace BIEngine {

const ComponentId PlayerComponent::g_CompId = "PlayerComponent";

bool PlayerComponent::Init(tinyxml2::XMLElement* pData)
{
   return true;
}

tinyxml2::XMLElement* PlayerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   return pBaseElement;
}

} // namespace BIEngine
