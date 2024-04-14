#pragma once

#include "Animation.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class AnimationData : public IResourceExtraData {
   friend class AnimationResourceLoader;

public:
   AnimationData()
      : m_pAnimation(nullptr)
   {
   }

   virtual ~AnimationData() {}

   AnimationData(const AnimationData& orig) = delete;
   AnimationData& operator=(const AnimationData& orig) = delete;

   virtual std::string ToString() override { return "AnimationData"; }

   std::shared_ptr<Animation> GetAnimation() const { return m_pAnimation; }

protected:
   std::shared_ptr<Animation> m_pAnimation;
};

class AnimationResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.bi3dam"; }
};

} // namespace BIEngine
