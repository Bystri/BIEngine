#include "RenderItemsStorage.h"

namespace BIEngine {

void RenderItemsStorage::InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem)
{
   m_opaqueItems.PushBack(opaqueRitem);
}

void RenderItemsStorage::InsertDirectionalLightInfo(const DirectionalLightItem& dirLight)
{
   m_directionalLightItems.PushBack(dirLight);
}

void RenderItemsStorage::InsertPointLightInfo(const PointLightItem& pointLight)
{
   m_pointLightItems.PushBack(pointLight);
}

void RenderItemsStorage::InsertSpotLightInfo(const SpotLightItem& spotLight)
{
   m_spotLightItems.PushBack(spotLight);
}

void RenderItemsStorage::Clear()
{
   m_opaqueItems.Clear();

   m_directionalLightItems.Clear();
   m_pointLightItems.Clear();
   m_spotLightItems.Clear();
}

} // namespace BIEngine
