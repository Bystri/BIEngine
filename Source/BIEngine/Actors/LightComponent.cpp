#include "LightComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ImageLoader.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {
ComponentId DirectionalLightComponent::g_CompId = "DirectionalLightComponent";
ComponentId PointLightComponent::g_CompId = "PointLightComponent";
ComponentId SpotLightComponent::g_CompId = "SpotLightComponent";

/***********************************************************
 * DirectionalLightComponent
 ************************************************************/

bool DirectionalLightComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      pColorElement->QueryFloatAttribute("r", &m_color.r);
      pColorElement->QueryFloatAttribute("g", &m_color.g);
      pColorElement->QueryFloatAttribute("b", &m_color.b);
   }

   tinyxml2::XMLElement* pIrradianceElement = pData->FirstChildElement("Irradiance");
   if (pIrradianceElement) {
      pIrradianceElement->QueryFloatAttribute("i", &m_irradiance);
   }

   return true;
}

void DirectionalLightComponent::OnRenderObject(const GameTimer& gt)
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (!pTransformComponent) {
      return;
   }

   RenderItemsStorage::DirectionalLightItem dirLight;
   dirLight.direction = pTransformComponent->GetRotation();
   dirLight.color = m_color;
   dirLight.irradiance = m_irradiance;

   g_pApp->TryGetHumanView(0)->GetScene()->GetRenderItemsStorage()->InsertDirectionalLightInfo(dirLight);
}

tinyxml2::XMLElement* DirectionalLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pColorElement = pDoc->NewElement("Color");
   pColorElement->SetAttribute("r", m_color.r);
   pColorElement->SetAttribute("g", m_color.g);
   pColorElement->SetAttribute("b", m_color.b);
   pBaseElement->LinkEndChild(pColorElement);

   tinyxml2::XMLElement* pIrradianceElement = pDoc->NewElement("Irradiance");
   pIrradianceElement->SetAttribute("i", m_irradiance);
   pBaseElement->LinkEndChild(pIrradianceElement);

   return pBaseElement;
}

/***********************************************************
 * PointLightComponent
 ************************************************************/


bool PointLightComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      pColorElement->QueryFloatAttribute("r", &m_color.r);
      pColorElement->QueryFloatAttribute("g", &m_color.g);
      pColorElement->QueryFloatAttribute("b", &m_color.b);
   }

   tinyxml2::XMLElement* pIntensityEelemnt = pData->FirstChildElement("Intensity");
   if (pIntensityEelemnt) {
      pIntensityEelemnt->QueryFloatAttribute("i", &m_intensity);
   }

   return true;
}

void PointLightComponent::OnRenderObject(const GameTimer& gt)
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (!pTransformComponent) {
      return;
   }

   RenderItemsStorage::PointLightItem pointLight;
   pointLight.position = pTransformComponent->GetPosition();

   pointLight.intensity = m_intensity;
   pointLight.color = m_color;

   g_pApp->TryGetHumanView(0)->GetScene()->GetRenderItemsStorage()->InsertPointLightInfo(pointLight);
}

tinyxml2::XMLElement* PointLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pColorElement = pDoc->NewElement("Color");
   pColorElement->SetAttribute("r", m_color.r);
   pColorElement->SetAttribute("g", m_color.g);
   pColorElement->SetAttribute("b", m_color.b);
   pBaseElement->LinkEndChild(pColorElement);

   tinyxml2::XMLElement* pIntensityElement = pDoc->NewElement("Intensity");
   pIntensityElement->SetAttribute("i", m_intensity);
   pBaseElement->LinkEndChild(pIntensityElement);

   return pBaseElement;
}

/***********************************************************
 * SpotLightComponent
 ************************************************************/


bool SpotLightComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pAmbientElement = pData->FirstChildElement("Ambient");
   if (pAmbientElement) {
      pAmbientElement->QueryFloatAttribute("r", &m_ambient.r);
      pAmbientElement->QueryFloatAttribute("g", &m_ambient.g);
      pAmbientElement->QueryFloatAttribute("b", &m_ambient.b);
   }


   tinyxml2::XMLElement* pDiffuseElement = pData->FirstChildElement("Diffuse");
   if (pDiffuseElement) {
      pDiffuseElement->QueryFloatAttribute("r", &m_diffuse.r);
      pDiffuseElement->QueryFloatAttribute("g", &m_diffuse.g);
      pDiffuseElement->QueryFloatAttribute("b", &m_diffuse.b);
   }


   tinyxml2::XMLElement* pSpecularElement = pData->FirstChildElement("Specular");
   if (pSpecularElement) {
      pSpecularElement->QueryFloatAttribute("r", &m_specular.r);
      pSpecularElement->QueryFloatAttribute("g", &m_specular.g);
      pSpecularElement->QueryFloatAttribute("b", &m_specular.b);
   }

   tinyxml2::XMLElement* pAttenuationElement = pData->FirstChildElement("Attenuation");
   if (pAttenuationElement) {
      pAttenuationElement->QueryFloatAttribute("constant", &m_attenuationConstant);
      pAttenuationElement->QueryFloatAttribute("linear", &m_attenuationLinear);
      pAttenuationElement->QueryFloatAttribute("quadratic", &m_attenuationQuadratic);
   }

   tinyxml2::XMLElement* pCutOffElement = pData->FirstChildElement("cutOff");
   if (pCutOffElement) {
      pCutOffElement->QueryFloatAttribute("innerCutOff", &m_cutOff);
      pCutOffElement->QueryFloatAttribute("outterCurOff", &m_outerCutOff);
   }

   return true;
}

void SpotLightComponent::OnRenderObject(const GameTimer& gt)
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (!pTransformComponent) {
      return;
   }

   RenderItemsStorage::SpotLightItem spotLight;
   spotLight.position = pTransformComponent->GetPosition();
   spotLight.direction = pTransformComponent->GetRotation();

   spotLight.constant = m_attenuationConstant;
   spotLight.linear = m_attenuationLinear;
   spotLight.quadratic = m_attenuationQuadratic;

   spotLight.ambient = m_ambient;
   spotLight.diffuse = m_diffuse;
   spotLight.specular = m_specular;

   spotLight.cutOff = m_cutOff;
   spotLight.outerCutOff = m_outerCutOff;

   g_pApp->TryGetHumanView(0)->GetScene()->GetRenderItemsStorage()->InsertSpotLightInfo(spotLight);
}

tinyxml2::XMLElement* SpotLightComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pAmbientElement = pDoc->NewElement("Ambient");
   pAmbientElement->SetAttribute("r", m_ambient.r);
   pAmbientElement->SetAttribute("g", m_ambient.g);
   pAmbientElement->SetAttribute("b", m_ambient.b);
   pBaseElement->LinkEndChild(pAmbientElement);

   tinyxml2::XMLElement* pDiffuseElement = pDoc->NewElement("Diffuse");
   pDiffuseElement->SetAttribute("r", m_diffuse.r);
   pDiffuseElement->SetAttribute("g", m_diffuse.g);
   pDiffuseElement->SetAttribute("b", m_diffuse.b);
   pBaseElement->LinkEndChild(pDiffuseElement);

   tinyxml2::XMLElement* pSpecularElement = pDoc->NewElement("Specular");
   pSpecularElement->SetAttribute("r", m_specular.r);
   pSpecularElement->SetAttribute("g", m_specular.g);
   pSpecularElement->SetAttribute("b", m_specular.b);
   pBaseElement->LinkEndChild(pSpecularElement);

   tinyxml2::XMLElement* pAttenuationElement = pDoc->NewElement("Attenuation");
   pAttenuationElement->SetAttribute("constant", m_attenuationConstant);
   pAttenuationElement->SetAttribute("linear", m_attenuationLinear);
   pAttenuationElement->SetAttribute("quadratic", m_attenuationQuadratic);
   pBaseElement->LinkEndChild(pAttenuationElement);

   tinyxml2::XMLElement* pCurOff = pDoc->NewElement("cutOff");
   pCurOff->SetAttribute("innerCutOff", m_cutOff);
   pCurOff->SetAttribute("outterCurOff", m_outerCutOff);
   pBaseElement->LinkEndChild(pCurOff);

   return pBaseElement;
}

} // namespace BIEngine
