#include "ShadowGraphicsTechnique.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"

namespace BIEngine {

bool ShadowGraphicsTechnique::Init()
{
   const std::string commonDirShadowShaderProgramPath = "effects/dirShadow.sp";
   auto dirShadowShaderProgram = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonDirShadowShaderProgramPath)->GetExtra());
   m_pDirLightShadowShader = dirShadowShaderProgram->GetShaderProgram();

   const std::string commonPointShadowShaderProgramPath = "effects/pointShadow.sp";
   auto pointShadowShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPointShadowShaderProgramPath)->GetExtra());
   m_pPointLightShadowShader = pointShadowShaderProgramData->GetShaderProgram();

   const std::string commonDirShadowSkinnedShaderProgramPath = "effects/dirShadowSkinned.sp";
   auto dirShadowShaderSkinnedProgram = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonDirShadowSkinnedShaderProgramPath)->GetExtra());
   m_pDirLightShadowSkinnedShader = dirShadowShaderSkinnedProgram->GetShaderProgram();

   const std::string commonPointShadowSkinnedShaderProgramPath = "effects/pointShadowSkinned.sp";
   auto pointShadowSkinnedShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPointShadowSkinnedShaderProgramPath)->GetExtra());
   m_pPointLightShadowSkinnedShader = pointShadowSkinnedShaderProgramData->GetShaderProgram();

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
         dirLightShadowInfo.pDepthBuffer = Texture2D::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, Texture::SizedFormat::DEPTH_COMPONENT, Texture::Format::DEPTH_COMPONENT, nullptr, params);
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

   return true;
}

void ShadowGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   const glm::mat4 dirProjMatr = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, -15.0f, 100.0f);

   const auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   const auto& opaqueAnimatedItems = pStorage->GetOpaqueAnimatedRenderItems();
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
      dirLights[i].pShadowMap = dirLightShadowInfo.pDepthBuffer;

      m_pDirLightShadowShader->Use();
      m_pDirLightShadowShader->SetMatrix4("lightSpaceMatrix", dirLights[i].LightMatr);

      for (const auto& ritem : opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh->GetVao(), ritem.pMesh->GetIndices().size(), m_pDirLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }

      m_pDirLightShadowSkinnedShader->Use();
      m_pDirLightShadowSkinnedShader->SetMatrix4("lightSpaceMatrix", dirLights[i].LightMatr);

      for (const auto& ritem : opaqueAnimatedItems) {
         for (int j = 0; j < ritem.boneMatrices.size(); ++j) {
            m_pDirLightShadowSkinnedShader->SetMatrix4("finalBonesMatrices[" + std::to_string(j) + "]", ritem.boneMatrices[j]);
         }

         RenderCommand renderCommand(ritem.pMesh->GetVao(), ritem.pMesh->GetIndices().size(), m_pDirLightShadowSkinnedShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }

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

      pointLight.pShadowMap = pointLightShadowInfo.pDepthBuffer;

      m_pPointLightShadowShader->Use();

      for (unsigned int j = 0; j < 6; ++j) {
         m_pPointLightShadowShader->SetMatrix4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
      }
      m_pPointLightShadowShader->SetVector3f("lightPos", pointLight.position);


      for (const auto& ritem : opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh->GetVao(), ritem.pMesh->GetIndices().size(), m_pPointLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }

      m_pPointLightShadowSkinnedShader->Use();

      for (unsigned int j = 0; j < 6; ++j) {
         m_pPointLightShadowSkinnedShader->SetMatrix4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
      }
      m_pPointLightShadowSkinnedShader->SetVector3f("lightPos", pointLight.position);


      for (const auto& ritem : opaqueAnimatedItems) {
         for (int j = 0; j < ritem.boneMatrices.size(); ++j) {
            m_pPointLightShadowSkinnedShader->SetMatrix4("finalBonesMatrices[" + std::to_string(j) + "]", ritem.boneMatrices[j]);
         }

         RenderCommand renderCommand(ritem.pMesh->GetVao(), ritem.pMesh->GetIndices().size(), m_pPointLightShadowSkinnedShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }
}

} // namespace BIEngine