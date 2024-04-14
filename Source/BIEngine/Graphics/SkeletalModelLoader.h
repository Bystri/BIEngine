#pragma once

#include "SkeletalModel.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class SkeletalModelData : public IResourceExtraData {
   friend class SkeletalModelResourceLoader;

public:
   SkeletalModelData()
      : m_pModel(nullptr)
   {
   }

   virtual ~SkeletalModelData() {}

   SkeletalModelData(const SkeletalModelData& orig) = delete;
   SkeletalModelData& operator=(const SkeletalModelData& orig) = delete;

   virtual std::string ToString() override { return "SkeletalModelData"; }

   std::shared_ptr<SkeletalModel> GetSkeletalModel() const { return m_pModel; }

protected:
   std::shared_ptr<SkeletalModel> m_pModel;
};

class SkeletalModelResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.glb"; }
};

} // namespace BIEngine
