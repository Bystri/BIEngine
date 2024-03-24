#include "ImageLoader.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../3rdParty/stb/stb_image.h"

namespace BIEngine {

ImageExtraData::ImageExtraData()
   : m_width(-1), m_height(-1), m_pData(nullptr)
{
}

ImageExtraData::~ImageExtraData()
{
   // Освобождаем ресурсы изображения
   stbi_image_free(m_pData);
}

bool ImageResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   // Создаем объект текстуры
   std::shared_ptr<ImageExtraData> pExtra = std::make_shared<ImageExtraData>();

   // Загружаем изображение
   int width, height, nrChannels;
   stbi_set_flip_vertically_on_load(true);
   unsigned char* pData = stbi_load_from_memory((stbi_uc*)pRawBuffer, rawSize, &width, &height, &nrChannels, 0);

   // Генерируем текстуру
   pExtra->m_width = width;
   pExtra->m_height = height;
   pExtra->m_channelsNum = nrChannels;
   pExtra->m_pData = pData;

   pHandle->SetExtra(pExtra);

   return true;
}

unsigned int ImageResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
   return rawSize;
}
} // namespace BIEngine
