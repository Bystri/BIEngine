#include "GraphicsRenderPass.h"

namespace BIEngine {

bool GraphicsRenderPass::Init()
{
   for (int i = 0; i < m_graphicsTechniques.size(); ++i) {
      m_graphicsTechniques[i]->Init();
   }

   return InitInternal();
}

void GraphicsRenderPass::AddTechnique(std::shared_ptr<IGraphicsTechnique> pTechnique)
{
   m_graphicsTechniques.push_back(pTechnique);
}

void GraphicsRenderPass::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   PreRender(pScene);

   for (int i = 0; i < m_graphicsTechniques.size(); ++i) {
      m_graphicsTechniques[i]->OnRender(pScene, pStorage);
   }

   PostRender(pScene);
}

} // namespace BIEngine
