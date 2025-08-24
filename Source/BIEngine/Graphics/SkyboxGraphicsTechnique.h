#pragma once

#include "Skybox.h"
#include "GraphicsTechnique.h"

namespace BIEngine {

class SkyboxGraphicsTechnique : public IGraphicsTechnique {
public:
   explicit SkyboxGraphicsTechnique(SharedPtr<Skybox> pSkybox)
      : IGraphicsTechnique(),
        m_pSkybox(pSkybox)
   {
   }

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   SharedPtr<Skybox> m_pSkybox;
};

} // namespace BIEngine
