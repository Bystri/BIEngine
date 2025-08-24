#pragma once

#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class OggResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return true; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle) { return true; }

   virtual String GetPattern() { return "*.ogg"; }
};

class WavResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return true; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle) { return true; }

   virtual String GetPattern() { return "*.wav"; }
};

} // namespace BIEngine
