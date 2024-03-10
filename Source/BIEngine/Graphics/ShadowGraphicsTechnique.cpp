#include "ShadowGraphicsTechnique.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"

namespace BIEngine {

void ShadowGraphicsTechnique::Init()
{
   const std::string commonDirShadowShaderProgramPath = "effects/dirShadow.sp";
   auto dirShadowShaderProgram = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonDirShadowShaderProgramPath)->GetExtra());
   m_pDirLightShadowShader = dirShadowShaderProgram->GetShaderProgram();

   const std::string commonPointShadowShaderProgramPath = "effects/pointShadow.sp";
   auto pointShadowShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPointShadowShaderProgramPath)->GetExtra());
   m_pPointLightShadowShader = pointShadowShaderProgramData->GetShaderProgram();

   for (int i = 0; i < m_maxDirLightsNum; ++i) {
      RenderDirLightShadowInfo dirLightShadowInfo;
      dirLightShadowInfo.pShadowMapBuffer = std::make_shared<Framebuffer>();

      {
         dirLightShadowInfo.pColorRenderbuffer = Renderbuffer::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Renderbuffer::Format::RGB8);
         FramebufferAttach(dirLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::COLOR, dirLightShadowInfo.pColorRenderbuffer);
      }
      {
         Texture2D::CreationParams params = Texture2D::CreationParams();
         params.WrapS = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.WrapT = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.FilterMin = Texture2D::TextureFunction::NEAREST;
         params.FilterMax = Texture2D::TextureFunction::NEAREST;
         params.DataType = Texture2D::Type::UNSIGNED_BYTE;
         dirLightShadowInfo.pDepthBuffer = Texture2D::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::Format::DEPTH_COMPONENT, nullptr, params);
         FramebufferAttach(dirLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::DEPTH, dirLightShadowInfo.pDepthBuffer);
      }

      m_dirLightShadowInfos.push_back(dirLightShadowInfo);
   }

   for (int i = 0; i < m_maxPointLightsNum; ++i) {
      std::array<unsigned char*, 6> data;
      for (int j = 0; j < data.size(); ++j) {
         data[j] = nullptr;
      }

      RenderPointLightShadowInfo pointLightShadowInfo;
      pointLightShadowInfo.pShadowMapBuffer = std::make_shared<Framebuffer>();

      {
         CubemapTexture::CreationParams params = CubemapTexture::CreationParams();
         params.WrapS = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.WrapT = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.WrapR = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.FilterMin = CubemapTexture::TextureFunction::NEAREST;
         params.FilterMax = CubemapTexture::TextureFunction::NEAREST;
         params.DataType = CubemapTexture::Type::UNSIGNED_BYTE;
         pointLightShadowInfo.pColorBuffer = CubemapTexture::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::Format::RGB, data, params);
         FramebufferAttach(pointLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::COLOR, pointLightShadowInfo.pColorBuffer);
      }

      {
         CubemapTexture::CreationParams params = CubemapTexture::CreationParams();
         params.WrapS = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.WrapT = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.WrapR = CubemapTexture::TextureWrap::CLAMP_TO_EDGE;
         params.FilterMin = CubemapTexture::TextureFunction::NEAREST;
         params.FilterMax = CubemapTexture::TextureFunction::NEAREST;
         params.DataType = CubemapTexture::Type::FLOAT;
         pointLightShadowInfo.pDepthBuffer = CubemapTexture::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::Format::DEPTH_COMPONENT, data, params);
         FramebufferAttach(pointLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::DEPTH, pointLightShadowInfo.pDepthBuffer);
      }

      m_pointLightShadowInfos.push_back(pointLightShadowInfo);
   }
}

void ShadowGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   m_pDirLightShadowShader->Use();

   const glm::mat4 dirProjMatr = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, -15.0f, 100.0f);

   const auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   auto& dirLights = pStorage->GetDirectionalLightItems();
   auto& pointLights = pStorage->GetPointLightItems();
   const auto& spotLights = pStorage->GetSpotLightItems();

   for (int i = 0; i < dirLights.size(); ++i) {
      RenderDirLightShadowInfo& dirLightShadowInfo = m_dirLightShadowInfos[i];

      dirLightShadowInfo.pShadowMapBuffer->Bind();
      glViewport(0, 0, m_dirLightShadowInfos[i].pDepthBuffer->GetWidth(), m_dirLightShadowInfos[i].pDepthBuffer->GetHeight());

      static const ColorRgba CLEAR_COLOR = ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
      pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

      const glm::mat4 viewMatr = glm::lookAt(glm::normalize(-dirLights[i].direction) * 20.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      dirLights[i].LightMatr = dirProjMatr * viewMatr;
      m_pDirLightShadowShader->SetMatrix4("lightSpaceMatrix", dirLights[i].LightMatr);
      dirLights[i].pShadowMap = dirLightShadowInfo.pDepthBuffer;

      for (const auto& ritem : opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh.get(), m_pDirLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }

   m_pPointLightShadowShader->Use();

   const float aspect = (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT;
   constexpr float near = 1.0f;
   constexpr float far = 25.0f;
   const glm::mat4 pointProjMatr = glm::perspective(glm::radians(90.0f), aspect, near, far);

   for (int i = 0; i < pointLights.size(); ++i) {
      RenderItemsStorage::PointLightItem& pointLight = pointLights[i];

      std::vector<glm::mat4> shadowTransforms;
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

      RenderPointLightShadowInfo& pointLightShadowInfo = m_pointLightShadowInfos[i];
      pointLightShadowInfo.pShadowMapBuffer->Bind();
      glViewport(0, 0, pointLightShadowInfo.pDepthBuffer->GetWidth(), pointLightShadowInfo.pDepthBuffer->GetHeight());
      static const ColorRgba CLEAR_COLOR = ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
      pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

      for (unsigned int i = 0; i < 6; ++i) {
         m_pPointLightShadowShader->SetMatrix4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
      }
      m_pPointLightShadowShader->SetVector3f("lightPos", pointLight.position);

      pointLight.pShadowMap = pointLightShadowInfo.pDepthBuffer;

      for (const auto& ritem : opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh.get(), m_pPointLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }
}

} // namespace BIEngine
