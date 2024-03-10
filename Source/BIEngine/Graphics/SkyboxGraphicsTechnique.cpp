#include "SkyboxGraphicsTechnique.h"

namespace BIEngine {

void SkyboxGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   m_pSkybox->OnRender(pScene);
}

} // namespace BIEngine
