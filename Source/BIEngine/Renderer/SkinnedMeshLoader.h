#pragma once

#include "../../ResourceCache/ResCache.h"
#include "SkinnedMesh.h"

namespace BIEngine {

class SkinnedMeshExtraData : public IResourceExtraData {
   friend class SkinnedMeshResourceLoader;

public:
   SkinnedMeshExtraData();
   virtual ~SkinnedMeshExtraData();

   virtual std::string ToString() { return "SkinnedMeshExtraData"; }

   std::shared_ptr<SkinnedMesh> GetSkinnedMesh() const { return m_pSkinnedMesh; }

protected:
   std::shared_ptr<SkinnedMesh> m_pSkinnedMesh;
};

class SkinnedMeshResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.biskmesh"; }
};

} // namespace BIEngine
