#pragma once

#include "../../ResourceCache/ResCache.h"
#include "Texture.h"

namespace BIEngine {

// TODO: Надо добавить конструкторы/операторы копирования из-за своеобразного деструктора
class ImageExtraData : public IResourceExtraData {
   friend class ImageResourceLoader;

public:
   ImageExtraData();
   virtual ~ImageExtraData();

   virtual std::string ToString() { return "ImageExtraData"; }

   int GetWidth() const { return m_width; }

   int GetHeight() const { return m_height; }

   unsigned char* GetData() const { return m_pData; }

protected:
   int m_width;
   int m_height;
   unsigned char* m_pData;
};

class ImageResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);
};

class PngResourceLoader : public ImageResourceLoader {
public:
   virtual std::string GetPattern() { return "*.png"; }
};

class JpgResourceLoader : public ImageResourceLoader {
public:
   virtual std::string GetPattern() { return "*.jpg"; }
};

} // namespace BIEngine
