#pragma once

#include <glad/glad.h>

#include "../../ResourceCache/ResCache.h"

namespace BIEngine
{

	//TODO: Надо добавить конструкторы/операторы копирования из-за своеобразного деструктора
	class OpenglShaderData : public IResourceExtraData
	{
		friend class VertexShaderResourceLoader;
		friend class FragmentShaderResourceLoader;

	public:
		OpenglShaderData();
		~OpenglShaderData() { glDeleteShader(m_shaderIndex); }
		virtual std::string ToString() { return "OpenglShaderData"; }

		unsigned int GetShaderIndex() const { return m_shaderIndex; }

	protected:
		unsigned int m_shaderIndex;
	};

	class ShaderResourceLoader : public IResourceLoader
	{
	public:
		virtual bool UseRawFile() { return false; }
		virtual bool DiscardRawBufferAfterLoad() { return true; }
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) { return 0; }
		virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) = 0;

	protected:
		void CheckCompileErrors(unsigned int object, std::string type);
	};



	class VertexShaderResourceLoader : public ShaderResourceLoader
	{
	public:
		virtual std::string GetPattern() { return "*.vs"; }
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);
	};


	class FragmentShaderResourceLoader : public ShaderResourceLoader
	{
	public:
		virtual std::string GetPattern() { return "*.frag"; }
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);
	};

}