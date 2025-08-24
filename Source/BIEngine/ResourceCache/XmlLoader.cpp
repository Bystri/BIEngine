#pragma once

#include "XmlLoader.h"

namespace BIEngine {

bool XmlResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<XmlExtraData> pExtra = MakeShared<XmlExtraData>();
   pExtra->m_xmlDoc.Parse(pRawBuffer, rawSize);

   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
