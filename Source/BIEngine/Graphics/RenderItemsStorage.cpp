#include "RenderItemsStorage.h"

namespace BIEngine {

void RenderItemsStorage::InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem)
{
   m_opaqueItems.push_back(opaqueRitem);
}

void RenderItemsStorage::InsertDirectionalLightInfo(const DirectionalLightItem& dirLight)
{
   m_directionalLightItems.push_back(dirLight);
}

void RenderItemsStorage::InsertPointLightInfo(const PointLightItem& pointLight)
{
   m_pointLightItems.push_back(pointLight);
}

void RenderItemsStorage::InsertSpotLightInfo(const SpotLightItem& spotLight)
{
   m_spotLightItems.push_back(spotLight);
}

void RenderItemsStorage::Clear()
{
   m_opaqueItems.clear();

   m_directionalLightItems.clear();
   m_pointLightItems.clear();
   m_spotLightItems.clear();
}

} // namespace BIEngine
