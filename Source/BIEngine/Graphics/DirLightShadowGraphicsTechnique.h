#pragma once

#include "GraphicsTechnique.h"

namespace BIEngine {

class DirLightShadowGraphicsTechnique : public IGraphicsTechnique {
public:
   DirLightShadowGraphicsTechnique(const std::size_t maxDirLightsNum)
      : IGraphicsTechnique(),
        m_maxDirLightsNum(maxDirLightsNum),
        SHADOW_MAP_WIDTH(1024), SHADOW_MAP_HEIGHT(1024),
        m_pDirLightShadowShader(nullptr),
        m_dirLightShadowInfos()
   {
   }

   bool Init() override;

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   struct RenderDirLightShadowInfo {
      std::shared_ptr<Framebuffer> pShadowMapBuffer;
      std::shared_ptr<Texture2D> pDepthBuffer;
   };

private:
   const std::size_t m_maxDirLightsNum;

   const int SHADOW_MAP_WIDTH;
   const int SHADOW_MAP_HEIGHT;

   std::shared_ptr<ShaderProgram> m_pDirLightShadowShader;
   std::vector<RenderDirLightShadowInfo> m_dirLightShadowInfos;
};

} // namespace BIEngine
