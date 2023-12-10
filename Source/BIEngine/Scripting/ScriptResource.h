#pragma once

#include "../ResourceCache/ResCache.h"

namespace BIEngine {

// Загружает и тут же выполняет Python-скрипт
class ScriptResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual bool AddNullZero() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) { return rawSize; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pRandle);

   virtual std::string GetPattern() { return "*.py"; }
};

} // namespace BIEngine
