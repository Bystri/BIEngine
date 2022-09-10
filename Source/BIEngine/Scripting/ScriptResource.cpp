#include "ScriptResource.h"

#include "LuaStateManager.h"

namespace BIEngine
{

    bool ScriptResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
    {
        if (rawSize <= 0)
            return false;

        LuaStateManager::Get()->ExecuteString(pRawBuffer);

        return true;
    }

}

