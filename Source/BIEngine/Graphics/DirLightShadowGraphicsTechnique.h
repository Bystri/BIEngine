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
      SharedPtr<Framebuffer> pShadowMapBuffer;
      SharedPtr<Texture2D> pDepthBuffer;
   };

private:
   const std::size_t m_maxDirLightsNum;

   const int SHADOW_MAP_WIDTH;
   const int SHADOW_MAP_HEIGHT;

   SharedPtr<ShaderProgram> m_pDirLightShadowShader;
   DynamicArray<RenderDirLightShadowInfo> m_dirLightShadowInfos;
};

} // namespace BIEngine
