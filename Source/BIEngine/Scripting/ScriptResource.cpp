#include "ScriptResource.h"

#include "PythonStateManager.h"

namespace BIEngine
{

    bool ScriptResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
    {
        if (rawSize <= 0)
            return false;

        PythonStateManager::Get()->ExecuteString(pRawBuffer);

        return true;
    }

}

