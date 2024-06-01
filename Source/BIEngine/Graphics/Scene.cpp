#include "Scene.h"

#include "GraphicsRenderPass.h"

namespace BIEngine {

Scene::Scene(std::shared_ptr<Renderer> pRenderer)
   : m_pRenderer(pRenderer), m_pConstantsBuffer(std::make_shared<ConstantsBuffer>()), m_pCamera(nullptr),
     m_graphicsRenderPasses(), m_pRenderItemsStorage(std::make_unique<RenderItemsStorage>())
{
}

void Scene::Init()
{
   constexpr int CONSTANTS_BUFFER_SCENE_GLOBALS_BINDING_POINT = 0;

   m_pConstantsBuffer->Init(sizeof(GlobalRenderBufferData), CONSTANTS_BUFFER_SCENE_GLOBALS_BINDING_POINT);
}

int Scene::OnPreRender(const GameTimer& gt)
{
   if (m_pCamera) {
      m_globalRenderBufferData.viewMat = m_pCamera->GetViewMatrix();
      m_globalRenderBufferData.projMat = m_pCamera->GetProjMatrix();
      m_globalRenderBufferData.viewPos = m_pCamera->GetPosition();
      m_globalRenderBufferData.totalTime = gt.TotalTime();
      m_pConstantsBuffer->SetBufferData(&m_globalRenderBufferData, 0, sizeof(m_globalRenderBufferData));

      m_pRenderItemsStorage->Clear();
   }

   return 0;
}

int Scene::OnPostRender(const GameTimer& gt)
{
   for (int i = 0; i < m_graphicsRenderPasses.size(); ++i) {
      m_graphicsRenderPasses[i]->OnRender(this, m_pRenderItemsStorage.get());
   }

   return 0;
}

} // namespace BIEngine
