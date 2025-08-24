#pragma once

#include "../../ResourceCache/ResCache.h"
#include "Mesh.h"

namespace BIEngine {

class MeshExtraData : public IResourceExtraData {
   friend class MeshResourceLoader;

public:
   MeshExtraData();
   virtual ~MeshExtraData();

   virtual String ToString() { return "MeshExtraData"; }

   SharedPtr<Mesh> GetMesh() const { return m_pMesh; }

protected:
   SharedPtr<Mesh> m_pMesh;
};

class MeshResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle);

   virtual String GetPattern() { return "*.bimesh"; }
};

} // namespace BIEngine
