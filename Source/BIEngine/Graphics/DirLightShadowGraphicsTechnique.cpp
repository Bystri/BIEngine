#include "DirLightShadowGraphicsTechnique.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"

namespace BIEngine {

bool DirLightShadowGraphicsTechnique::Init()
{
   const std::string commonDirShadowShaderProgramPath = "effects/dirShadow.bisp";
   auto dirShadowShaderProgram = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonDirShadowShaderProgramPath)->GetExtra());
   m_pDirLightShadowShader = dirShadowShaderProgram->GetShaderProgram();

   for (int i = 0; i < m_maxDirLightsNum; ++i) {
      RenderDirLightShadowInfo dirLightShadowInfo;
      dirLightShadowInfo.pShadowMapBuffer = std::make_shared<Framebuffer>();

      FramebufferDisableColor(dirLightShadowInfo.pShadowMapBuffer, FramebufferColorOperationType::ALL);

      {
         Texture2D::CreationParams params = Texture2D::CreationParams();
         params.WrapS = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.WrapT = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.FilterMin = Texture2D::TextureFunction::NEAREST;
         params.FilterMax = Texture2D::TextureFunction::NEAREST;
         params.DataType = Texture2D::Type::UNSIGNED_BYTE;
         dirLightShadowInfo.pDepthBuffer = Texture2D::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::SizedFormat::DEPTH_COMPONENT, Texture::Format::DEPTH_COMPONENT, nullptr, params);
         FramebufferAttach(dirLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::DEPTH, dirLightShadowInfo.pDepthBuffer);
      }

      m_dirLightShadowInfos.push_back(dirLightShadowInfo);
   }

   return true;
}

void DirLightShadowGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   const glm::mat4 dirProjMatr = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, -15.0f, 100.0f);

   const auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   auto& dirLights = pStorage->GetDirectionalLightItems();

   for (int i = 0; i < dirLights.size(); ++i) {
      RenderDirLightShadowInfo& dirLightShadowInfo = m_dirLightShadowInfos[i];

      dirLightShadowInfo.pShadowMapBuffer->Bind();
      glViewport(0, 0, m_dirLightShadowInfos[i].pDepthBuffer->GetWidth(), m_dirLightShadowInfos[i].pDepthBuffer->GetHeight());

      static const ColorRgba CLEAR_COLOR = ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
      pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

      const glm::mat4 viewMatr = glm::lookAt(glm::normalize(-dirLights[i].direction) * 20.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      dirLights[i].LightMatr = dirProjMatr * viewMatr;
      dirLights[i].pShadowMap = dirLightShadowInfo.pDepthBuffer;

      m_pDirLightShadowShader->Use();
      m_pDirLightShadowShader->SetMatrix4("lightSpaceMatrix", dirLights[i].LightMatr);

      for (const auto& ritem : opaqueItems) {
         RenderCommand renderCommand(ritem.VAO, ritem.IndicesSize, m_pDirLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }
}

} // namespace BIEngine
