#pragma once

#include "GraphicsTechnique.h"

namespace BIEngine {

class ShadowGraphicsTechnique : public IGraphicsTechnique {
public:
   ShadowGraphicsTechnique(const std::size_t maxDirLightsNum, const std::size_t maxPointLightsNum)
      : IGraphicsTechnique(),
        m_maxDirLightsNum(maxDirLightsNum), m_maxPointLightsNum(maxPointLightsNum),
        SHADOW_MAP_WIDTH(1024), SHADOW_MAP_HEIGHT(1024),
        m_pDirLightShadowShader(nullptr), m_pPointLightShadowShader(nullptr),
        m_pDirLightShadowSkinnedShader(nullptr), m_pPointLightShadowSkinnedShader(nullptr),
        m_dirLightShadowInfos(), m_pointLightShadowInfos()
   {
   }

   bool Init() override;

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;

private:
   struct RenderDirLightShadowInfo {
      std::shared_ptr<Framebuffer> pShadowMapBuffer;
      std::shared_ptr<Renderbuffer> pColorRenderbuffer;
      std::shared_ptr<Texture2D> pDepthBuffer;
   };

   struct RenderPointLightShadowInfo {
      std::shared_ptr<Framebuffer> pShadowMapBuffer;
      std::shared_ptr<CubemapTexture> pColorBuffer;
      std::shared_ptr<CubemapTexture> pDepthBuffer;
   };

private:
   const std::size_t m_maxDirLightsNum;
   const std::size_t m_maxPointLightsNum;

   const int SHADOW_MAP_WIDTH;
   const int SHADOW_MAP_HEIGHT;

   std::shared_ptr<ShaderProgram> m_pDirLightShadowShader;
   std::shared_ptr<ShaderProgram> m_pPointLightShadowShader;

   std::shared_ptr<ShaderProgram> m_pDirLightShadowSkinnedShader;
   std::shared_ptr<ShaderProgram> m_pPointLightShadowSkinnedShader;

   std::vector<RenderDirLightShadowInfo> m_dirLightShadowInfos;
   std::vector<RenderPointLightShadowInfo> m_pointLightShadowInfos;
};

} // namespace BIEngine
