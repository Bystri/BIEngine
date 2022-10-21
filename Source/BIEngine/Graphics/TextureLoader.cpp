#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../3rdParty/stb/stb_image.h"

namespace BIEngine
{

	unsigned int TextureResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
	{
		//Мы не выделяем память в кэше для текстуры
		return 0;
	}


	TextureExtraData::TextureExtraData()
		: m_pTexture(nullptr)
	{
	}

	bool PngResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
	{
		//Создаем объект текстуры
		std::shared_ptr<TextureExtraData> pExtra = std::make_shared<TextureExtraData>();
		pExtra->m_pTexture = std::make_shared<Texture2D>();

		//TODO: Сделать возможность загружать текстуры без альфа канала
		pExtra->m_pTexture->SetInternalFormat(GL_RGBA);
		pExtra->m_pTexture->SetImageFormat(GL_RGBA);

		//Загружаем изображение
		int width, height, nrChannels;
		unsigned char* pData = stbi_load_from_memory((stbi_uc*)pRawBuffer, rawSize, &width, &height, &nrChannels, 0);
		
		// Генерируем текстуру
		pExtra->m_pTexture->Generate(width, height, pData);
		
		//Освобождаем ресурсы изображения
		stbi_image_free(pData);
		pHandle->SetExtra(pExtra);

		return true;
	}

}