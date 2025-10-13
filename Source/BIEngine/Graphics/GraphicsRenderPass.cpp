#include "GraphicsRenderPass.h"

namespace BIEngine {

bool GraphicsRenderPass::Init()
{
   for (int i = 0; i < m_graphicsTechniques.Size(); ++i) {
      if (!m_graphicsTechniques[i]->Init()) {
         return false;
      }
   }

   return InitInternal();
}

void GraphicsRenderPass::AddTechnique(SharedPtr<IGraphicsTechnique> pTechnique)
{
   m_graphicsTechniques.PushBack(pTechnique);
}

void GraphicsRenderPass::OnRender(Scene* const pScene, const GameTimer& gt, RenderItemsStorage* const pStorage)
{
   PreRender(pScene, gt);

   for (int i = 0; i < m_graphicsTechniques.Size(); ++i) {
      m_graphicsTechniques[i]->OnRender(pScene, pStorage);
   }

   PostRender(pScene,gt);
}

} // namespace BIEngine
