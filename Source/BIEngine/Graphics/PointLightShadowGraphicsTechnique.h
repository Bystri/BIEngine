#pragma once

#include "GraphicsTechnique.h"

namespace BIEngine {

class PointLightShadowGraphicsTechnique : public IGraphicsTechnique {
public:
   PointLightShadowGraphicsTechnique(const std::size_t maxPointLightsNum)
      : IGraphicsTechnique(),
        m_maxPointLightsNum(maxPointLightsNum),
        SHADOW_MAP_WIDTH(1024), SHADOW_MAP_HEIGHT(1024),
        m_pPointLightShadowShader(nullptr),
        m_pointLightShadowInfos()
   {
   }

   bool Init() override;

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   struct RenderPointLightShadowInfo {
      SharedPtr<Framebuffer> pShadowMapBuffer;
      SharedPtr<CubemapTexture> pColorBuffer;
      SharedPtr<Texture2D> pDepthBuffer;
   };

private:
   const std::size_t m_maxPointLightsNum;

   const int SHADOW_MAP_WIDTH;
   const int SHADOW_MAP_HEIGHT;

   SharedPtr<ShaderProgram> m_pPointLightShadowShader;
   DynamicArray<RenderPointLightShadowInfo> m_pointLightShadowInfos;
};

} // namespace BIEngine
