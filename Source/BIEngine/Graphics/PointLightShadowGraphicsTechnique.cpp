#include "PointLightShadowGraphicsTechnique.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"

namespace BIEngine {

bool PointLightShadowGraphicsTechnique::Init()
{
   const std::string commonPointShadowShaderProgramPath = "effects/pointShadow.bisp";
   auto pointShadowShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPointShadowShaderProgramPath)->GetExtra());
   m_pPointLightShadowShader = pointShadowShaderProgramData->GetShaderProgram();

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
         params.FilterMin = CubemapTexture::TextureFunction::LINEAR;
         params.FilterMax = CubemapTexture::TextureFunction::LINEAR;
         params.DataType = CubemapTexture::Type::FLOAT;
         pointLightShadowInfo.pColorBuffer = CubemapTexture::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, BIEngine::CubemapTexture::SizedFormat::R_32_F, Texture::Format::RED, data, params);

         FramebufferDisableColor(pointLightShadowInfo.pShadowMapBuffer, FramebufferColorOperationType::ALL);
      }

      {
         Texture2D::CreationParams params = Texture2D::CreationParams();
         params.WrapS = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.WrapT = Texture2D::TextureWrap::CLAMP_TO_BORDER;
         params.FilterMin = Texture2D::TextureFunction::NEAREST;
         params.FilterMax = Texture2D::TextureFunction::NEAREST;
         params.DataType = Texture2D::Type::UNSIGNED_BYTE;
         pointLightShadowInfo.pDepthBuffer = Texture2D::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::SizedFormat::DEPTH_COMPONENT, Texture::Format::DEPTH_COMPONENT, nullptr, params);

         FramebufferAttach(pointLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::DEPTH, pointLightShadowInfo.pDepthBuffer);
      }

      m_pointLightShadowInfos.push_back(pointLightShadowInfo);
   }

   return true;
}

void PointLightShadowGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   const auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   auto& pointLights = pStorage->GetPointLightItems();

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

      glViewport(0, 0, pointLightShadowInfo.pDepthBuffer->GetWidth(), pointLightShadowInfo.pDepthBuffer->GetHeight());

      pointLight.pShadowMap = pointLightShadowInfo.pColorBuffer;

      for (int sideIdx = 0; sideIdx < shadowTransforms.size(); ++sideIdx) {
         FramebufferAttach(pointLightShadowInfo.pShadowMapBuffer, FramebufferAttachementType::COLOR, pointLightShadowInfo.pColorBuffer, sideIdx);
         FramebufferEnableColor(pointLightShadowInfo.pShadowMapBuffer, FramebufferColorOperationType::DRAW);

         constexpr float MAX = std::numeric_limits<float>::max();
         static const ColorRgba CLEAR_COLOR = ColorRgba(MAX, MAX, MAX, MAX);
         pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

         m_pPointLightShadowShader->Use();

         m_pPointLightShadowShader->SetMatrix4("shadowMatrix", shadowTransforms[sideIdx]);
         m_pPointLightShadowShader->SetVector3f("lightPos", pointLight.position);


         for (const auto& ritem : opaqueItems) {
            RenderCommand renderCommand(ritem.VAO, ritem.IndicesSize, m_pPointLightShadowShader);
            renderCommand.Transform = ritem.ModelTransform;
            pScene->GetRenderer()->DrawRenderCommand(renderCommand);
         }
      }
   }
}

} // namespace BIEngine
