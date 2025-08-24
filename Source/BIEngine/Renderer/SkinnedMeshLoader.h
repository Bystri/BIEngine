#pragma once

#include "../../ResourceCache/ResCache.h"
#include "SkinnedMesh.h"

namespace BIEngine {

class SkinnedMeshExtraData : public IResourceExtraData {
   friend class SkinnedMeshResourceLoader;

public:
   SkinnedMeshExtraData();
   virtual ~SkinnedMeshExtraData();

   virtual String ToString() { return "SkinnedMeshExtraData"; }

   SharedPtr<SkinnedMesh> GetSkinnedMesh() const { return m_pSkinnedMesh; }

protected:
   SharedPtr<SkinnedMesh> m_pSkinnedMesh;
};

class SkinnedMeshResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle);

   virtual String GetPattern() { return "*.biskmesh"; }
};

} // namespace BIEngine
