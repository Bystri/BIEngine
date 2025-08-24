#pragma once

#include "Model.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class ModelData : public IResourceExtraData {
   friend class ModelResourceLoader;

public:
   ModelData()
      : m_pModel(nullptr)
   {
   }

   virtual ~ModelData() {}

   ModelData(const ModelData& orig) = delete;
   ModelData& operator=(const ModelData& orig) = delete;

   virtual String ToString() override { return "ModelData"; }

   SharedPtr<Model> GetModel() const { return m_pModel; }

protected:
   SharedPtr<Model> m_pModel;
};

class ModelResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle);

   virtual String GetPattern() { return "*.obj"; }
};

} // namespace BIEngine
