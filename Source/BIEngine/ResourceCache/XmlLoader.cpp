#pragma once

#include "XmlLoader.h"

namespace BIEngine {

bool XmlResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<XmlExtraData> pExtra = std::make_shared<XmlExtraData>();
   pExtra->m_xmlDoc.Parse(pRawBuffer, rawSize);

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine