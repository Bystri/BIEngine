#pragma once

#include "Texture.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class TextureData : public IResourceExtraData {
   friend class TextureResourceLoader;

public:
   TextureData()
      : m_pTexture(nullptr)
   {
   }

   virtual ~TextureData() {}

   TextureData(const TextureData& orig) = delete;
   TextureData& operator=(const TextureData& orig) = delete;

   virtual std::string ToString() override { return "TextureData"; }

   std::shared_ptr<Texture2D> GetTexture() const { return m_pTexture; }

protected:
   std::shared_ptr<Texture2D> m_pTexture;
};

class TextureResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.bitf"; }
};

} // namespace BIEngine
