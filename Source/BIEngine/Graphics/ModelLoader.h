#pragma once

#include "Model.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class ModelData : public IResourceExtraData {
   friend class ObjModelResourceLoader;

public:
   ModelData()
      : m_pModel(nullptr)
   {
   }

   virtual ~ModelData() {}

   // Удаляем их, так как шейдеры используются только для генерации шейдер-программы,
   // и маловероятно событие, что нам нужно будет копирование, чтобы тратиться на реализацию поддержки контроля копирования
   ModelData(const ModelData& orig) = delete;
   ModelData& operator=(const ModelData& orig) = delete;

   virtual std::string ToString() override { return "ModelData"; }

   std::shared_ptr<Model> GetModel() const { return m_pModel; }

protected:
   std::shared_ptr<Model> m_pModel;
};

class ObjModelResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return true; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);

   virtual std::string GetPattern() { return "*.obj"; }
};

} // namespace BIEngine
