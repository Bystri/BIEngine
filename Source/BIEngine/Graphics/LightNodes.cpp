#include "LightNodes.h"

#include <cstdint>

#include "Scene.h"

namespace BIEngine {

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
constexpr int MAX_POINT_LIGHTS_NUM = 16;
constexpr int MAX_SPOT_LIGHTS_NUM = 12;

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
   explicit LightManager()
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

LightManagerNode::LightManagerNode()
   : SceneNode(Actor::INVALID_ACTOR_ID, RenderLayer::LIGHT)
{
   if (g_pLightManager == nullptr) {
      g_pLightManager = new LightManager();
   }
}

LightManagerNode::~LightManagerNode()
{
   if (g_pLightManager != nullptr) {
      delete g_pLightManager;
      g_pLightManager = nullptr;
   }
}

bool LightManagerNode::RenderChildren(Scene* pScene)
{
   g_pLightManager->Clear();
   SceneNode::RenderChildren(pScene);
   g_pLightManager->ApplyLightData();

   return true;
}

bool DirectionalLightNode::OnRender(Scene* pScene)
{
   DirectionalLightGlData dirLights;
   dirLights.direction = GetRotation();
   dirLights.ambient = GetAmbient();
   dirLights.diffuse = GetDiffuse();
   dirLights.specular = GetSpecular();

   g_pLightManager->InsertDirectionalLightInfo(dirLights);
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
   return true;
}

} // namespace BIEngine
