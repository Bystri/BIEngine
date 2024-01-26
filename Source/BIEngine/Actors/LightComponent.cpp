#include "LightComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ImageLoader.h"

namespace BIEngine {
ComponentId DirectionalLightComponent::g_CompId = "DirectionalLightComponent";
ComponentId PointLightComponent::g_CompId = "PointLightComponent";
ComponentId SpotLightComponent::g_CompId = "SpotLightComponent";

/***********************************************************
 * DirectionalLightComponent
 ************************************************************/

bool DirectionalLightComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pDirectionalLightNode == nullptr) {
      m_pDirectionalLightNode = std::make_shared<DirectionalLightNode>(m_pOwner->GetId());
   }

   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pColorElement->QueryFloatAttribute("r", &r);
      pColorElement->QueryFloatAttribute("g", &g);
      pColorElement->QueryFloatAttribute("b", &b);
      m_pDirectionalLightNode->SetColor(glm::vec3(r, g, b));
   }

   tinyxml2::XMLElement* pIrradianceElement = pData->FirstChildElement("Irradiance");
   if (pIrradianceElement) {
      float i = 0;
      pIrradianceElement->QueryFloatAttribute("i", &i);
      m_pDirectionalLightNode->SetIrradiance(i);
   }

   return true;
}

tinyxml2::XMLElement* DirectionalLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pColorElement = pDoc->NewElement("Ambient");
   const glm::vec3& color = m_pDirectionalLightNode->GetColor();
   pColorElement->SetAttribute("r", color.r);
   pColorElement->SetAttribute("g", color.g);
   pColorElement->SetAttribute("b", color.b);
   pBaseElement->LinkEndChild(pColorElement);

   tinyxml2::XMLElement* pIrradianceElement = pDoc->NewElement("Irradiance");
   pIrradianceElement->SetAttribute("i", m_pDirectionalLightNode->GetIrradiance());
   pBaseElement->LinkEndChild(pIrradianceElement);

   return pBaseElement;
}

std::shared_ptr<SceneNode> DirectionalLightComponent::CreateSceneNode()
{
   if (m_pDirectionalLightNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pDirectionalLightNode->SetTransform(pTransformComponent);
   }

   return m_pDirectionalLightNode;
}

/***********************************************************
 * PointLightComponent
 ************************************************************/


bool PointLightComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pPointLightNode == nullptr) {
      m_pPointLightNode = std::make_shared<PointLightNode>(m_pOwner->GetId());
   }

   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pColorElement->QueryFloatAttribute("r", &r);
      pColorElement->QueryFloatAttribute("g", &g);
      pColorElement->QueryFloatAttribute("b", &b);
      m_pPointLightNode->SetColor(glm::vec3(r, g, b));
   }


   tinyxml2::XMLElement* pIntensityEelemnt = pData->FirstChildElement("Intensity");
   if (pIntensityEelemnt) {
      float intensity = 0;
      pIntensityEelemnt->QueryFloatAttribute("i", &intensity);
      m_pPointLightNode->SetIntensity(intensity);
   }

   return true;
}

tinyxml2::XMLElement* PointLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pColorElement = pDoc->NewElement("Color");
   const glm::vec3& color = m_pPointLightNode->GetColor();
   pColorElement->SetAttribute("r", color.r);
   pColorElement->SetAttribute("g", color.g);
   pColorElement->SetAttribute("b", color.b);
   pBaseElement->LinkEndChild(pColorElement);

   tinyxml2::XMLElement* pIntensityElement = pDoc->NewElement("Intensity");
   pIntensityElement->SetAttribute("i", m_pPointLightNode->GetIntensity());
   pBaseElement->LinkEndChild(pIntensityElement);

   return pBaseElement;
}

std::shared_ptr<SceneNode> PointLightComponent::CreateSceneNode()
{
   if (m_pPointLightNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pPointLightNode->SetTransform(pTransformComponent);
   }

   return m_pPointLightNode;
}

/***********************************************************
 * SpotLightComponent
 ************************************************************/


bool SpotLightComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pSpotLightNode == nullptr) {
      m_pSpotLightNode = std::make_shared<SpotLightNode>(m_pOwner->GetId());
   }

   tinyxml2::XMLElement* pAmbientElement = pData->FirstChildElement("Ambient");
   if (pAmbientElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pAmbientElement->QueryFloatAttribute("r", &r);
      pAmbientElement->QueryFloatAttribute("g", &g);
      pAmbientElement->QueryFloatAttribute("b", &b);
      m_pSpotLightNode->SetAmbient(glm::vec3(r, g, b));
   }


   tinyxml2::XMLElement* pDiffuseElement = pData->FirstChildElement("Diffuse");
   if (pDiffuseElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pDiffuseElement->QueryFloatAttribute("r", &r);
      pDiffuseElement->QueryFloatAttribute("g", &g);
      pDiffuseElement->QueryFloatAttribute("b", &b);
      m_pSpotLightNode->SetDiffuse(glm::vec3(r, g, b));
   }


   tinyxml2::XMLElement* pSpecularElement = pData->FirstChildElement("Specular");
   if (pSpecularElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pSpecularElement->QueryFloatAttribute("r", &r);
      pSpecularElement->QueryFloatAttribute("g", &g);
      pSpecularElement->QueryFloatAttribute("b", &b);
      m_pSpotLightNode->SetSpecular(glm::vec3(r, g, b));
   }

   tinyxml2::XMLElement* pAttenuationElement = pData->FirstChildElement("Attenuation");
   if (pAttenuationElement) {
      float constant = 0;
      float linear = 0;
      float quadratic = 0;
      pAttenuationElement->QueryFloatAttribute("constant", &constant);
      pAttenuationElement->QueryFloatAttribute("linear", &linear);
      pAttenuationElement->QueryFloatAttribute("quadratic", &quadratic);
      m_pSpotLightNode->SetAttenuationParams(constant, linear, quadratic);
   }

   tinyxml2::XMLElement* pCutOffElement = pData->FirstChildElement("cutOff");
   if (pCutOffElement) {
      float innerCutOff = 0;
      float outterCurOff = 0;
      pCutOffElement->QueryFloatAttribute("innerCutOff", &innerCutOff);
      pCutOffElement->QueryFloatAttribute("outterCurOff", &outterCurOff);
      m_pSpotLightNode->SetCutOff(innerCutOff);
      m_pSpotLightNode->SetOuterCutOff(outterCurOff);
   }

   return true;
}

tinyxml2::XMLElement* SpotLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pAmbientElement = pDoc->NewElement("Ambient");
   pAmbientElement->SetAttribute("r", m_pSpotLightNode->GetAmbient().r);
   pAmbientElement->SetAttribute("g", m_pSpotLightNode->GetAmbient().g);
   pAmbientElement->SetAttribute("b", m_pSpotLightNode->GetAmbient().b);
   pBaseElement->LinkEndChild(pAmbientElement);

   tinyxml2::XMLElement* pDiffuseElement = pDoc->NewElement("Diffuse");
   pDiffuseElement->SetAttribute("r", m_pSpotLightNode->GetDiffuse().r);
   pDiffuseElement->SetAttribute("g", m_pSpotLightNode->GetDiffuse().g);
   pDiffuseElement->SetAttribute("b", m_pSpotLightNode->GetDiffuse().b);
   pBaseElement->LinkEndChild(pDiffuseElement);

   tinyxml2::XMLElement* pSpecularElement = pDoc->NewElement("Specular");
   pSpecularElement->SetAttribute("r", m_pSpotLightNode->GetSpecular().r);
   pSpecularElement->SetAttribute("g", m_pSpotLightNode->GetSpecular().g);
   pSpecularElement->SetAttribute("b", m_pSpotLightNode->GetSpecular().b);
   pBaseElement->LinkEndChild(pAmbientElement);

   tinyxml2::XMLElement* pAttenuationElement = pDoc->NewElement("Attenuation");
   pAttenuationElement->SetAttribute("constant", m_pSpotLightNode->GetAttenuationConstant());
   pAttenuationElement->SetAttribute("linear", m_pSpotLightNode->GetAttenuationLinear());
   pAttenuationElement->SetAttribute("quadratic", m_pSpotLightNode->GetAttenuationQuadratic());
   pBaseElement->LinkEndChild(pAttenuationElement);

   tinyxml2::XMLElement* pCurOff = pDoc->NewElement("cutOff");
   pCurOff->SetAttribute("innerCutOff", m_pSpotLightNode->GetCutOff());
   pCurOff->SetAttribute("outterCurOff", m_pSpotLightNode->GetOuterCutOff());
   pBaseElement->LinkEndChild(pCurOff);

   return pBaseElement;
}

std::shared_ptr<SceneNode> SpotLightComponent::CreateSceneNode()
{
   if (m_pSpotLightNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pSpotLightNode->SetTransform(pTransformComponent);
   }

   return m_pSpotLightNode;
}

} // namespace BIEngine
