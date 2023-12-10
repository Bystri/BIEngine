#pragma once

#include "../ResourceCache/ResCache.h"

#include <memory>

namespace BIEngine {

class OggResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return true; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) { return true; }

   virtual std::string GetPattern() { return "*.ogg"; }
};

class WavResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return true; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) { return true; }

   virtual std::string GetPattern() { return "*.wav"; }
};

} // namespace BIEngine
