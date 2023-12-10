#pragma once

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "../../ResourceCache/ResCache.h"

namespace BIEngine {

class UtilityShaderData : public IResourceExtraData {
   friend class UtilityShaderResourceLoader;

public:
   UtilityShaderData()
      : m_utilityShaderSource() {}

   virtual ~UtilityShaderData() {}

   // Удаляем их, так как шейдеры используются только для генерации шейдер-программы,
   // и маловероятно событие, что нам нужно будет копирование, чтобы тратиться на реализацию поддержки контроля копирования
   UtilityShaderData(const UtilityShaderData& orig) = delete;
   UtilityShaderData& operator=(const UtilityShaderData& orig) = delete;

   virtual std::string ToString() override { return "UtilityShaderData"; }

   std::string GetUtilityShaderSource() const { return m_utilityShaderSource; }

protected:
   std::string m_utilityShaderSource;
};

class UtilityShaderResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() override { return false; }

   virtual bool DiscardRawBufferAfterLoad() override { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) override { return 0; }

   virtual std::string GetPattern() override { return "*.glsl"; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) override;
};

class ShaderData : public IResourceExtraData {
   friend class VertexShaderResourceLoader;
   friend class FragmentShaderResourceLoader;

public:
   ShaderData();

   virtual ~ShaderData() { glDeleteShader(m_shaderIndex); }

   // Удаляем их, так как шейдеры используются только для генерации шейдер-программы,
   // и маловероятно событие, что нам нужно будет копирование, чтобы тратиться на реализацию поддержки контроля копирования
   ShaderData(const ShaderData& orig) = delete;
   ShaderData& operator=(const ShaderData& orig) = delete;

   virtual std::string ToString() override { return "ShaderData"; }

   unsigned int GetShaderIndex() const { return m_shaderIndex; }

protected:
   unsigned int m_shaderIndex;
};

class ShaderProgramData : public IResourceExtraData {
   friend class ShaderProgramResourceLoader;

public:
   ShaderProgramData() {}

   virtual ~ShaderProgramData() {}

   ShaderProgramData(const ShaderData& orig) = delete;
   ShaderProgramData& operator=(const ShaderData& orig) = delete;

   virtual std::string ToString() override { return "ShaderProgramData"; }

   std::shared_ptr<ShaderProgram> GetShaderProgram() const { return m_pShaderProgram; }

protected:
   std::shared_ptr<ShaderProgram> m_pShaderProgram;
};

class ShaderResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() override { return false; }

   virtual bool DiscardRawBufferAfterLoad() override { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) override { return 0; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) override = 0;
};

class VertexShaderResourceLoader : public ShaderResourceLoader {
public:
   virtual std::string GetPattern() override { return "*.vs"; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) override;
};

class FragmentShaderResourceLoader : public ShaderResourceLoader {
public:
   virtual std::string GetPattern() override { return "*.frag"; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) override;
};

class ShaderProgramResourceLoader : public IResourceLoader {
public:
   virtual std::string GetPattern() override { return "*.sp"; }

   virtual bool UseRawFile() override { return false; }

   virtual bool DiscardRawBufferAfterLoad() override { return true; }

   virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) override { return 0; }

   virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) override;
};

} // namespace BIEngine
