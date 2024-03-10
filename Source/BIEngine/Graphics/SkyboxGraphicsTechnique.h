#pragma once

#include "Skybox.h"
#include "GraphicsTechnique.h"

namespace BIEngine {

class SkyboxGraphicsTechnique : public IGraphicsTechnique {
public:
   explicit SkyboxGraphicsTechnique(std::shared_ptr<Skybox> pSkybox)
      : IGraphicsTechnique(),
        m_pSkybox(pSkybox)
   {
   }

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   std::shared_ptr<Skybox> m_pSkybox;
};

} // namespace BIEngine
