#include "ScriptResource.h"

#include "PythonStateManager.h"

namespace BIEngine {

bool ScriptResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   if (rawSize <= 0)
      return false;

   PythonStateManager::Get()->ExecuteString(pRawBuffer);

   return true;
}

} // namespace BIEngine
