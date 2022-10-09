#pragma once

#include "../../ResourceCache/ResCache.h"
#include "Texture.h"

namespace BIEngine
{

	//TODO: Надо добавить конструкторы/операторы копирования из-за своеобразного деструктора
	class TextureExtraData : public IResourceExtraData
	{
		friend class AlphaPngResourceLoader;
		friend class PngResourceLoader;

	public:
		TextureExtraData();
		virtual ~TextureExtraData() { }
		virtual std::string ToString() { return "TextureExtraData"; }

		std::shared_ptr<Texture2D> GetTexture() const { return m_pTexture; }

	protected:
		std::shared_ptr<Texture2D> m_pTexture;
	};


	class TextureResourceLoader : public IResourceLoader
	{
	public:
		virtual bool UseRawFile() { return false; }
		virtual bool DiscardRawBufferAfterLoad() { return true; }
		virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle) = 0;
	};

	class PngResourceLoader : public TextureResourceLoader
	{
	public:
		virtual std::string GetPattern() { return "*.png"; }
		virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle);
	};

}
