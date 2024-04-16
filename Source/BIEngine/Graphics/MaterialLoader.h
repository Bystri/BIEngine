#pragma once

#include "Material.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class MaterialData : public IResourceExtraData {
   friend class MaterialResourceLoader;

public:
   MaterialData()
      : m_pMaterial(nullptr)
   {
   }

   virtual ~MaterialData() {}

   MaterialData(const MaterialData& orig) = delete;
   MaterialData& operator=(const MaterialData& orig) = delete;

   virtual std::string ToString() override { return "MaterialData"; }

   std::shared_ptr<Material> GetMaterial() const { return m_pMaterial; }

protected:
   std::shared_ptr<Material> m_pMaterial;
};

class MaterialResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.bimat"; }
};

} // namespace BIEngine
