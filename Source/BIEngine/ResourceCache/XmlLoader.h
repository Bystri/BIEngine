#pragma once

#include <tinyxml2.h>

#include "../../ResourceCache/ResCache.h"

namespace BIEngine {

// TODO: Надо добавить конструкторы/операторы копирования из-за своеобразного деструктора
class XmlExtraData : public IResourceExtraData {
   friend class XmlResourceLoader;

public:
   XmlExtraData()
      : m_xmlDoc(){};

   virtual ~XmlExtraData() {}

   virtual std::string ToString() { return "XmlExtraData"; }

   tinyxml2::XMLElement* GetRootElement() { return m_xmlDoc.RootElement(); }

protected:
   tinyxml2::XMLDocument m_xmlDoc;
};

class XmlResourceLoader : public IResourceLoader {
public:
   virtual std::string GetPattern() { return "*.xml"; }

   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) { return 0; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);
};

} // namespace BIEngine
