#include "SceneNodes.h"

#include "Model.h"
#include "Scene.h"
#include "../EngineCore/GameApp.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"

namespace BIEngine {

glm::mat4 SceneNode::GetLocalModelMatrix() const
{
   if (m_pParent) {
      return m_pParent->GetLocalModelMatrix() * m_props.GetTransformMatrix();
   }

   return m_props.GetTransformMatrix();
}

bool SceneNode::OnUpdate(Scene* pScene, const GameTimer& gt)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->OnUpdate(pScene, gt);
   }

   return true;
}

bool SceneNode::PreRender(Scene* pScene)
{
   return true;
}

bool SceneNode::PostRender(Scene* pScene)
{
   return true;
}

bool SceneNode::RenderChildren(Scene* pScene)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      if ((*itr)->PreRender(pScene)) {
         if ((*itr)->IsVisible(pScene)) {
            (*itr)->OnRender(pScene);
         }

         (*itr)->RenderChildren(pScene);
      }
      (*itr)->PostRender(pScene);
   }

   return true;
}

void SceneNode::AddChild(std::shared_ptr<ISceneNode> pChild)
{
   m_children.push_back(pChild);

   // TODO: Что насчет, если наш ребенок будет БОЛЬШЕ родителя? Что насчет удаления?
}

void SceneNode::RemoveChild(ActorId id)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      const SceneNodeProperties* pProps = (*itr)->Get();

      (*itr)->RemoveChild(id);

      if (id == pProps->GetActorId()) {
         itr = m_children.erase(itr);
      }
   }
}

struct DirectionalLightGlData {
   glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);
   uint32_t pan1;
   glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
   uint32_t pan2;
   glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
   uint32_t pan3;
   glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
   uint32_t pan4;
};

struct PointLightGlData {
   glm::vec3 position = glm::vec3(0.0f);
   float constant = 1.0f;
   glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
   float linear = 0.09f;
   glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
   float quadratic = 0.032f;
   glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
   uint32_t pan7;
};

struct SpotLightGlData {
   glm::vec3 position = glm::vec3(0.0f);
   float constant = 1.0f;
   glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
   float linear = 0.09f;
   glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
   float quadratic = 0.032f;
   glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
   float cutOff = glm::cos(glm::radians(12.5f));
   glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
   float outerCutOff = glm::cos(glm::radians(25.0f));
};

constexpr int MAX_DIRECTIONAL_LIGHTS_NUM = 1;
constexpr int MAX_POINT_LIGHTS_NUM = 1;
constexpr int MAX_SPOT_LIGHTS_NUM = 1;
constexpr int SHADOW_MAP_WIDTH = 1024;
constexpr int SHADOW_MAP_HEIGHT = 1024;

struct LightUniformBufferData {
   int32_t DirectionalLightNum;
   int32_t PointLightsNum;
   int32_t SpotLightsNum;
   uint32_t pan1;
   DirectionalLightGlData DirLights[MAX_DIRECTIONAL_LIGHTS_NUM];
   PointLightGlData PointLights[MAX_POINT_LIGHTS_NUM];
   SpotLightGlData SpotLights[MAX_SPOT_LIGHTS_NUM];
};

class LightManager {
public:
   LightManager()
      : m_pLightConstantsBuffer(std::make_shared<ConstantsBuffer>())
   {
      constexpr int CONSTANTS_BUFFER_LIGHTS_BINDING_POINT = 1;
      m_pLightConstantsBuffer->Init(sizeof(LightUniformBufferData), CONSTANTS_BUFFER_LIGHTS_BINDING_POINT);
   }

   void InsertDirectionalLightInfo(const DirectionalLightGlData& dirLight);
   void InsertPointLightInfo(const PointLightGlData& pointLight);
   void InsertSpotLightInfo(const SpotLightGlData& spotLight);

   void ApplyLightData();
   void Clear();

private:
   LightUniformBufferData m_lightsBufferData;
   std::shared_ptr<ConstantsBuffer> m_pLightConstantsBuffer;
};

void LightManager::InsertDirectionalLightInfo(const DirectionalLightGlData& dirLight)
{
   assert(m_lightsBufferData.DirectionalLightNum < MAX_DIRECTIONAL_LIGHTS_NUM);
   if (m_lightsBufferData.DirectionalLightNum >= MAX_DIRECTIONAL_LIGHTS_NUM) {
      return;
   }

   m_lightsBufferData.DirLights[m_lightsBufferData.DirectionalLightNum] = dirLight;
   ++m_lightsBufferData.DirectionalLightNum;
}

void LightManager::InsertPointLightInfo(const PointLightGlData& pointLight)
{
   assert(m_lightsBufferData.PointLightsNum < MAX_POINT_LIGHTS_NUM);
   if (m_lightsBufferData.PointLightsNum >= MAX_POINT_LIGHTS_NUM) {
      return;
   }


   m_lightsBufferData.PointLights[m_lightsBufferData.PointLightsNum] = pointLight;
   ++m_lightsBufferData.PointLightsNum;
}

void LightManager::InsertSpotLightInfo(const SpotLightGlData& spotLight)
{
   assert(m_lightsBufferData.SpotLightsNum < MAX_SPOT_LIGHTS_NUM);
   if (m_lightsBufferData.SpotLightsNum >= MAX_SPOT_LIGHTS_NUM) {
      return;
   }

   m_lightsBufferData.SpotLights[m_lightsBufferData.SpotLightsNum] = spotLight;
   ++m_lightsBufferData.SpotLightsNum;
}

void LightManager::ApplyLightData()
{
   m_pLightConstantsBuffer->SetBufferData(&m_lightsBufferData, 0, sizeof(LightUniformBufferData));
}

void LightManager::Clear()
{
   m_lightsBufferData.DirectionalLightNum = 0;
   m_lightsBufferData.PointLightsNum = 0;
   m_lightsBufferData.SpotLightsNum = 0;
}

LightManager* g_pLightManager = nullptr;

class ShadowManager {
public:
   struct OpaqueRenderItem {
      std::shared_ptr<Mesh> pMesh;
      glm::mat4 ModelTransform;
   };

   struct RenderDirLightShadowInfo {
      std::shared_ptr<Framebuffer> pShadowMapBuffer;
      std::shared_ptr<Renderbuffer> pColorRenderbuffer;
      std::shared_ptr<Texture2D> pDepthBuffer;
      glm::mat4 LightMatr;
   };

   struct RenderPointLightShadowInfo {
      std::shared_ptr<Framebuffer> pShadowMapBuffer;
      std::shared_ptr<CubemapTexture> pColorBuffer;
      std::shared_ptr<CubemapTexture> pDepthBuffer;
      glm::vec3 LightPos;
   };

   ShadowManager()
      : m_pDirLightShadowShader(nullptr), m_pPointLightShadowShader(nullptr),
        m_dirLights(), m_pointLights(), m_spotLights(),
        m_dirLightShadowInfos(),
        m_opaqueItems()
   {
      const std::string commonDirShadowShaderProgramPath = "effects/dirShadow.sp";
      auto dirShadowShaderProgram = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonDirShadowShaderProgramPath)->GetExtra());
      m_pDirLightShadowShader = dirShadowShaderProgram->GetShaderProgram();

      const std::string commonPointShadowShaderProgramPath = "effects/pointShadow.sp";
      auto pointShadowShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(commonPointShadowShaderProgramPath)->GetExtra());
      m_pPointLightShadowShader = pointShadowShaderProgramData->GetShaderProgram();

      for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS_NUM; ++i) {
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

      for (int i = 0; i < MAX_POINT_LIGHTS_NUM; ++i) {
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

   const std::vector<RenderDirLightShadowInfo> GetDirLightShadowInfos() const { return m_dirLightShadowInfos; }

   const std::vector<RenderPointLightShadowInfo> GetPointLightShadowInfos() const { return m_pointLightShadowInfos; }

   void InsertDirectionalLightInfo(const DirectionalLightGlData& dirLight);
   void InsertPointLightInfo(const PointLightGlData& pointLight);
   void InsertSpotLightInfo(const SpotLightGlData& spotLight);

   void InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem);

   void ApplyShadowData(Scene* pScene);
   void Clear();

private:
   std::shared_ptr<ShaderProgram> m_pDirLightShadowShader;
   std::shared_ptr<ShaderProgram> m_pPointLightShadowShader;

   std::vector<DirectionalLightGlData> m_dirLights;
   std::vector<PointLightGlData> m_pointLights;
   std::vector<SpotLightGlData> m_spotLights;

   std::vector<RenderDirLightShadowInfo> m_dirLightShadowInfos;
   std::vector<RenderPointLightShadowInfo> m_pointLightShadowInfos;

   std::vector<OpaqueRenderItem> m_opaqueItems;
};

void ShadowManager::InsertDirectionalLightInfo(const DirectionalLightGlData& dirLight)
{
   assert(m_dirLights.size() < MAX_DIRECTIONAL_LIGHTS_NUM);
   if (m_dirLights.size() >= MAX_DIRECTIONAL_LIGHTS_NUM) {
      return;
   }

   m_dirLights.push_back(dirLight);
}

void ShadowManager::InsertPointLightInfo(const PointLightGlData& pointLight)
{
   assert(m_pointLights.size() < MAX_POINT_LIGHTS_NUM);
   if (m_pointLights.size() >= MAX_POINT_LIGHTS_NUM) {
      return;
   }

   m_pointLights.push_back(pointLight);
}

void ShadowManager::InsertSpotLightInfo(const SpotLightGlData& spotLight)
{
   assert(m_spotLights.size() < MAX_SPOT_LIGHTS_NUM);
   if (m_spotLights.size() >= MAX_SPOT_LIGHTS_NUM) {
      return;
   }

   m_spotLights.push_back(spotLight);
}

void ShadowManager::InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem)
{
   m_opaqueItems.push_back(opaqueRitem);
}

void ShadowManager::ApplyShadowData(Scene* pScene)
{
   m_pDirLightShadowShader->Use();

   const glm::mat4 dirProjMatr = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, -15.0f, 100.0f);

   for (int i = 0; i < m_dirLights.size(); ++i) {
      RenderDirLightShadowInfo& dirLightShadowInfo = m_dirLightShadowInfos[i];

      dirLightShadowInfo.pShadowMapBuffer->Bind();
      glViewport(0, 0, m_dirLightShadowInfos[i].pDepthBuffer->GetWidth(), m_dirLightShadowInfos[i].pDepthBuffer->GetHeight());

      static constexpr Color CLEAR_COLOR = Color(0.0f, 0.5f, 0.5f, 1.0f);
      pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

      const glm::mat4 viewMatr = glm::lookAt(glm::normalize(-m_dirLights[i].direction) * 20.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      dirLightShadowInfo.LightMatr = dirProjMatr * viewMatr;
      m_pDirLightShadowShader->SetMatrix4("lightSpaceMatrix", dirLightShadowInfo.LightMatr);

      for (const auto& ritem : m_opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh, m_pDirLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }

   m_pPointLightShadowShader->Use();

   constexpr float aspect = (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT;
   constexpr float near = 1.0f;
   constexpr float far = 25.0f;
   const glm::mat4 pointProjMatr = glm::perspective(glm::radians(90.0f), aspect, near, far);

   for (int i = 0; i < m_pointLights.size(); ++i) {
      const PointLightGlData& pointLight = m_pointLights[i];

      std::vector<glm::mat4> shadowTransforms;
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(pointProjMatr * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

      RenderPointLightShadowInfo& pointLightShadowInfo = m_pointLightShadowInfos[i];
      pointLightShadowInfo.LightPos = pointLight.position;
      pointLightShadowInfo.pShadowMapBuffer->Bind();
      glViewport(0, 0, pointLightShadowInfo.pDepthBuffer->GetWidth(), pointLightShadowInfo.pDepthBuffer->GetHeight());
      static constexpr Color CLEAR_COLOR = Color(0.0f, 0.5f, 0.5f, 1.0f);
      pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

      for (unsigned int i = 0; i < 6; ++i) {
         m_pPointLightShadowShader->SetMatrix4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
      }
      m_pPointLightShadowShader->SetVector3f("lightPos", pointLight.position);

      for (const auto& ritem : m_opaqueItems) {
         RenderCommand renderCommand(ritem.pMesh, m_pPointLightShadowShader);
         renderCommand.Transform = ritem.ModelTransform;
         pScene->GetRenderer()->DrawRenderCommand(renderCommand);
      }
   }
}

void ShadowManager::Clear()
{
   m_dirLights.clear();
   m_pointLights.clear();
   m_spotLights.clear();

   m_opaqueItems.clear();
}

ShadowManager* g_pShadowManager = nullptr;

class OpaqueRenderItemManager {
public:
   struct OpaqueRenderItem {
      std::shared_ptr<Mesh> pMesh;
      std::shared_ptr<Material> pMaterial;
      glm::mat4 ModelTransform;
   };

   OpaqueRenderItemManager()
      : m_opaqueItems()
   {
   }

   void InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem);

   void ApplyOpaqueItemsData(Scene* pScene);
   void Clear();

private:
   std::vector<OpaqueRenderItem> m_opaqueItems;
};

void OpaqueRenderItemManager::InsertOpaqueRenderItem(const OpaqueRenderItem& opaqueRitem)
{
   m_opaqueItems.push_back(opaqueRitem);
}

void OpaqueRenderItemManager::ApplyOpaqueItemsData(Scene* pScene)
{
   for (const auto& ritem : m_opaqueItems) {
      RenderCommand renderCommand(ritem.pMesh, ritem.pMaterial->GetShaderProgramPtr());

      renderCommand.RenderState = ritem.pMaterial->GetRenderState();
      renderCommand.RenderState.Cull = !ritem.pMaterial->IsDoubleSided();
      renderCommand.Transform = ritem.ModelTransform;

      renderCommand.GetShaderProgramState() = ritem.pMaterial->ConstructShaderProgramState();

      const auto& dirLightShadowInfos = g_pShadowManager->GetDirLightShadowInfos();
      for (int i = 0; i < dirLightShadowInfos.size(); ++i) {
         renderCommand.GetShaderProgramState().SetMatrix4("dirLightShadowInfos[" + std::to_string(i) + "].dirLightSpaceMatrix", dirLightShadowInfos[i].LightMatr);
         const int nextFreeTextureSlot = renderCommand.GetShaderProgramState().GetTexturesNum();
         renderCommand.GetShaderProgramState().SetInteger("dirLightShadowInfos[" + std::to_string(i) + "].shadowMap", nextFreeTextureSlot);
         renderCommand.GetShaderProgramState().AddTexture(dirLightShadowInfos[i].pDepthBuffer);
      }

      const auto& pointLightShadowInfos = g_pShadowManager->GetPointLightShadowInfos();
      for (int i = 0; i < pointLightShadowInfos.size(); ++i) {
         renderCommand.GetShaderProgramState().SetVector3f("pointLightShadowInfos[" + std::to_string(i) + "].lightPos", pointLightShadowInfos[i].LightPos);
         const int nextFreeTextureSlot = renderCommand.GetShaderProgramState().GetTexturesNum();
         renderCommand.GetShaderProgramState().SetInteger("pointLightShadowInfos[" + std::to_string(i) + "].shadowMap", nextFreeTextureSlot);
         renderCommand.GetShaderProgramState().AddTexture(pointLightShadowInfos[i].pDepthBuffer);
      }
      pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   }
}

void OpaqueRenderItemManager::Clear()
{
   m_opaqueItems.clear();
}

OpaqueRenderItemManager* g_pOpaqueRenderItemManager = nullptr;

bool DirectionalLightNode::OnRender(Scene* pScene)
{
   DirectionalLightGlData dirLight;
   dirLight.direction = GetRotation();
   dirLight.ambient = GetAmbient();
   dirLight.diffuse = GetDiffuse();
   dirLight.specular = GetSpecular();

   g_pLightManager->InsertDirectionalLightInfo(dirLight);
   g_pShadowManager->InsertDirectionalLightInfo(dirLight);
   return true;
}

bool PointLightNode::OnRender(Scene* pScene)
{
   PointLightGlData pointLight;
   pointLight.position = GetPosition();

   pointLight.constant = m_attenuationConstant;
   pointLight.linear = m_attenuationLinear;
   pointLight.quadratic = m_attenuationQuadratic;

   pointLight.ambient = GetAmbient();
   pointLight.diffuse = GetDiffuse();
   pointLight.specular = GetSpecular();

   g_pLightManager->InsertPointLightInfo(pointLight);
   g_pShadowManager->InsertPointLightInfo(pointLight);
   return true;
}

bool SpotLightNode::OnRender(Scene* pScene)
{
   SpotLightGlData spotLight;
   spotLight.position = GetPosition();
   spotLight.direction = GetRotation();

   spotLight.constant = m_attenuationConstant;
   spotLight.linear = m_attenuationLinear;
   spotLight.quadratic = m_attenuationQuadratic;

   spotLight.ambient = GetAmbient();
   spotLight.diffuse = GetDiffuse();
   spotLight.specular = GetSpecular();

   spotLight.cutOff = GetCutOff();
   spotLight.outerCutOff = GetOuterCutOff();

   g_pLightManager->InsertSpotLightInfo(spotLight);
   g_pShadowManager->InsertSpotLightInfo(spotLight);
   return true;
}

bool ModelNode::OnRender(Scene* pScene)
{
   const std::vector<std::shared_ptr<ModelMesh>>& modelMeshes = m_pModel->GetMeshes();

   for (const auto& pModelMesh : modelMeshes) {
      ShadowManager::OpaqueRenderItem shadowRitem;
      shadowRitem.pMesh = pModelMesh->GetMesh();
      shadowRitem.ModelTransform = GetLocalModelMatrix();
      g_pShadowManager->InsertOpaqueRenderItem(shadowRitem);

      OpaqueRenderItemManager::OpaqueRenderItem opaqueRitem;
      opaqueRitem.pMesh = pModelMesh->GetMesh();
      opaqueRitem.pMaterial = pModelMesh->GetMaterial();
      opaqueRitem.ModelTransform = GetLocalModelMatrix();
      g_pOpaqueRenderItemManager->InsertOpaqueRenderItem(opaqueRitem);
   }

   return true;
}

RootNode::RootNode()
   : SceneNode(Actor::INVALID_ACTOR_ID, RenderLayer::BEGIN)
{
   if (g_pLightManager == nullptr) {
      g_pLightManager = new LightManager();
   }

   if (g_pShadowManager == nullptr) {
      g_pShadowManager = new ShadowManager();
   }

   if (g_pOpaqueRenderItemManager == nullptr) {
      g_pOpaqueRenderItemManager = new OpaqueRenderItemManager();
   }

   m_children.reserve(static_cast<size_t>(RenderLayer::END));
   m_children.push_back(std::make_shared<SceneNode>(Actor::INVALID_ACTOR_ID, RenderLayer::LIGHT));
   m_children.push_back(std::make_shared<SceneNode>(Actor::INVALID_ACTOR_ID, RenderLayer::OPAQUE));
}

RootNode::~RootNode()
{
   if (g_pLightManager) {
      delete g_pLightManager;
   }

   if (g_pShadowManager) {
      delete g_pShadowManager;
   }

   if (g_pOpaqueRenderItemManager) {
      delete g_pOpaqueRenderItemManager;
   }
}

void RootNode::AddChild(std::shared_ptr<ISceneNode> pChild)
{
   RenderLayer pass = pChild->Get()->GetRenderLayer();
   if (static_cast<unsigned>(pass) >= m_children.size() || !m_children[static_cast<std::size_t>(pass)]) {
      assert(0 && "There is no such render pass");
      return;
   }

   m_children[static_cast<std::size_t>(pass)]->AddChild(pChild);
}

bool RootNode::RenderChildren(Scene* pScene)
{
   g_pLightManager->Clear();
   g_pShadowManager->Clear();
   g_pOpaqueRenderItemManager->Clear();

   for (int pass = static_cast<unsigned>(RenderLayer::BEGIN); pass < static_cast<unsigned>(RenderLayer::END); ++pass) {
      m_children[pass]->RenderChildren(pScene);
   }

   g_pLightManager->ApplyLightData();
   g_pShadowManager->ApplyShadowData(pScene);

   pScene->GetRenderer()->BeginFrame();
   glViewport(0, 0, pScene->GetRenderer()->GetScreenWidth(), pScene->GetRenderer()->GetScreenHeight());
   static constexpr Color CLEAR_COLOR = Color(0.0f, 0.5f, 0.5f, 1.0f);
   pScene->GetRenderer()->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

   g_pOpaqueRenderItemManager->ApplyOpaqueItemsData(pScene);

   return true;
}

} // namespace BIEngine
