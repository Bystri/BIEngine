#pragma once

#include "../../ResourceCache/ResCache.h"
#include "Mesh.h"

namespace BIEngine {

class MeshExtraData : public IResourceExtraData {
   friend class MeshResourceLoader;

public:
   MeshExtraData();
   virtual ~MeshExtraData();

   virtual std::string ToString() { return "MeshExtraData"; }

   std::shared_ptr<Mesh> GetMesh() const { return m_pMesh; }

protected:
   std::shared_ptr<Mesh> m_pMesh;
};

class MeshResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.bimesh"; }
};

} // namespace BIEngine
