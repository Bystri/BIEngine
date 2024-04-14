#include "RenderComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Renderer/ShadersLoader.h"
#include "../Graphics/ModelLoader.h"
#include "../Graphics/SkeletalModelLoader.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"
#include "../Renderer/TextureLoader.h"

namespace BIEngine {
ComponentId SpriteRenderComponent::g_CompId = "SpriteRenderComponent";
ComponentId BoxRenderComponent::g_CompId = "BoxRenderComponent";
ComponentId ModelRenderComponent::g_CompId = "ModelRenderComponent";
ComponentId SkeletalModelRenderComponent::g_CompId = "SkeletalModelRenderComponent";

/***********************************************************
 * BaseRenderComponent
 ************************************************************/

void BaseRenderComponent::Activate()
{
   std::shared_ptr<SceneNode> pSceneNode(GetSceneNode());
   std::shared_ptr<EvtData_New_Render_Component> pEvent = std::make_shared<EvtData_New_Render_Component>(m_pOwner->GetId(), pSceneNode);
   EventManager::Get()->TriggerEvent(pEvent);
}

std::shared_ptr<SceneNode> BaseRenderComponent::GetSceneNode()
{
   if (!m_pSceneNode)
      m_pSceneNode = CreateSceneNode();
   return m_pSceneNode;
}

/***********************************************************
 * MeshBaseRenderComponent
 ************************************************************/


bool MeshBaseRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   tinyxml2::XMLElement* pShaderProgramInfo = pData->FirstChildElement("ShaderProgram");

   if (pShaderProgramInfo == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading RenderComponent for Actor with id: " + std::to_string(m_pOwner->GetId()) + "; No Shader was specified");
      return false;
   }

   const char* shaderProgramPath;
   pShaderProgramInfo->QueryStringAttribute("shaderProgramPath", &shaderProgramPath);

   m_shaderProgrampath = shaderProgramPath;
   std::shared_ptr<ShaderProgramData> pShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(m_shaderProgrampath)->GetExtra());
   m_pLightReflectionMaterial = std::make_shared<LightReflectiveMaterial>(pShaderProgramData->GetShaderProgram());
   m_pLightReflectionMaterial->SetColor(COLOR_WHITE);

   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pColorElement->QueryFloatAttribute("r", &r);
      pColorElement->QueryFloatAttribute("g", &g);
      pColorElement->QueryFloatAttribute("b", &b);
      const ColorRgb matColor = ColorRgb(r, g, b);
      m_pLightReflectionMaterial->SetColor(matColor);
   }

   return true;
}

tinyxml2::XMLElement* MeshBaseRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pShader = pDoc->NewElement("ShaderProgram");
   pShader->SetAttribute("shaderProgramPath", m_shaderProgrampath.c_str());
   pBaseElement->LinkEndChild(pShader);

   tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
   pColor->SetAttribute("r", std::to_string(m_pLightReflectionMaterial->GetColor().r).c_str());
   pColor->SetAttribute("g", std::to_string(m_pLightReflectionMaterial->GetColor().g).c_str());
   pColor->SetAttribute("b", std::to_string(m_pLightReflectionMaterial->GetColor().b).c_str());
   pBaseElement->LinkEndChild(pColor);

   return pBaseElement;
}

/***********************************************************
 * SpriteRenderComponent
 ************************************************************/

bool SpriteRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pSpriteNode == nullptr) {
      m_pSpriteNode = std::make_shared<SpriteNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
   }

   ColorRgba spriteColor = COLOR_WHITE;

   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pColorElement->QueryFloatAttribute("r", &r);
      pColorElement->QueryFloatAttribute("g", &g);
      pColorElement->QueryFloatAttribute("b", &b);
      spriteColor = ColorRgba(r, g, b, 1.0f);
   }

   tinyxml2::XMLElement* pSpriteElement = pData->FirstChildElement("Sprite");
   if (pSpriteElement) {
      const char* spritePath;
      pSpriteElement->QueryStringAttribute("path", &spritePath);
      m_spritePath = spritePath;
      auto spriteData = std::static_pointer_cast<TextureData>(ResCache::Get()->GetHandle(spritePath)->GetExtra());

      if (spriteData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading sprite for Actor with id: " + std::to_string(m_pOwner->GetId()));
         m_pSpriteNode.reset();
         return false;
      }

      std::shared_ptr<Sprite> pSprite = std::make_shared<Sprite>(spriteData->GetTexture());
      pSprite->SetColor(spriteColor);
      m_pSpriteNode->SetSprite(pSprite);
   }

   return true;
}

tinyxml2::XMLElement* SpriteRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   const ColorRgba& spriteColor = m_pSpriteNode->GetSprite()->GetColor();

   tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
   pColor->SetAttribute("r", std::to_string(spriteColor.r).c_str());
   pColor->SetAttribute("g", std::to_string(spriteColor.g).c_str());
   pColor->SetAttribute("b", std::to_string(spriteColor.b).c_str());
   pBaseElement->LinkEndChild(pColor);


   tinyxml2::XMLElement* pSprite = pDoc->NewElement("Sprite");
   pSprite->SetAttribute("path", m_spritePath.c_str());
   pBaseElement->LinkEndChild(pSprite);

   return pBaseElement;
}

std::shared_ptr<SceneNode> SpriteRenderComponent::CreateSceneNode()
{
   if (m_pSpriteNode == nullptr)
      return std::shared_ptr<SceneNode>();

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pSpriteNode->SetTransform(pTransformComponent);
   }

   return m_pSpriteNode;
}

/***********************************************************
 * BoxRenderComponent
 ************************************************************/

bool BoxRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!MeshBaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pModelNode == nullptr) {
      m_pModelNode = std::make_shared<ModelNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
   }

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
   if (pSizeElement) {
      double w, h, d;
      pSizeElement->QueryDoubleAttribute("w", &w);
      pSizeElement->QueryDoubleAttribute("h", &h);
      pSizeElement->QueryDoubleAttribute("d", &d);

      m_width = (float)w;
      m_height = (float)h;
      m_depth = (float)d;
   }

   tinyxml2::XMLElement* pMaterialElement = pData->FirstChildElement("Material");
   if (pMaterialElement) {
      float shininess = 64.0f;
      pMaterialElement->QueryFloatAttribute("shininess", &shininess);
      m_pLightReflectionMaterial->SetShininess(shininess);

      bool isDoubleSided = false;
      pMaterialElement->QueryBoolAttribute("isDoubleSided", &isDoubleSided);
      m_pLightReflectionMaterial->SetDoubleSided(isDoubleSided);

      const char* diffuseMapPath;
      pMaterialElement->QueryStringAttribute("diffuseMapPath", &diffuseMapPath);
      m_diffuseMapPath = diffuseMapPath;
      auto diffuseMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(m_diffuseMapPath)->GetExtra());

      if (diffuseMapData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading diffuse map texture for Actor with id: " + std::to_string(m_pOwner->GetId()));
         m_pModelNode.reset();
         return false;
      }

      m_pLightReflectionMaterial->SetDiffuseMap(diffuseMapData->GetTexture());

      const char* specularMapPath;
      pMaterialElement->QueryStringAttribute("specularMapPath", &specularMapPath);
      m_specularMapPath = specularMapPath;
      auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(m_specularMapPath)->GetExtra());

      if (specularMapData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading specular map texture for Actor with id: " + std::to_string(m_pOwner->GetId()));
         m_pModelNode.reset();
         return false;
      }

      m_pLightReflectionMaterial->SetSpecularMap(specularMapData->GetTexture());

      const char* normalMapPath;
      pMaterialElement->QueryStringAttribute("normalMapPath", &normalMapPath);
      m_normalMapPath = normalMapPath;
      auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(m_normalMapPath)->GetExtra());

      if (normalMapData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading normal map texture for Actor with id: " + std::to_string(m_pOwner->GetId()));
         m_pModelNode.reset();
         return false;
      }

      m_pLightReflectionMaterial->SetNormalMap(normalMapData->GetTexture());

      const char* displacementMapPath;
      pMaterialElement->QueryStringAttribute("displacementMapPath", &displacementMapPath);
      m_displacementMapPath = displacementMapPath;
      auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle(m_displacementMapPath)->GetExtra());

      if (displacementMapData == nullptr) {
         Logger::WriteLog(Logger::LogType::ERROR, "Error while loading displacement map texture for Actor with id: " + std::to_string(m_pOwner->GetId()));
         m_pModelNode.reset();
         return false;
      }

      m_pLightReflectionMaterial->SetDisplacementMap(displacementMapData->GetTexture());

      std::shared_ptr<Mesh> boxMesh = std::make_shared<Mesh>(MeshGeometryGenerator::CreateBox(m_width, m_height, m_depth, 0u));
      std::shared_ptr<ModelMesh> pModelMesh = std::make_shared<ModelMesh>(boxMesh, m_pLightReflectionMaterial);

      std::shared_ptr<Model> pModel = std::make_shared<Model>();
      pModel->AddModelMesh(pModelMesh);
      m_pModelNode->SetModel(pModel);
   }

   return true;
}

tinyxml2::XMLElement* BoxRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = MeshBaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
   pSize->SetAttribute("w", std::to_string(m_width).c_str());
   pSize->SetAttribute("h", std::to_string(m_height).c_str());
   pSize->SetAttribute("d", std::to_string(m_depth).c_str());
   pBaseElement->LinkEndChild(pSize);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("shininess", m_pLightReflectionMaterial->GetShininess());
   pMaterialElement->SetAttribute("isDoubleSided", m_pLightReflectionMaterial->IsDoubleSided());
   pMaterialElement->SetAttribute("diffuseMapPath", m_diffuseMapPath.c_str());
   pMaterialElement->SetAttribute("specularMapPath", m_specularMapPath.c_str());
   pMaterialElement->SetAttribute("normalMapPath", m_normalMapPath.c_str());
   pMaterialElement->SetAttribute("m_displacementMapPath", m_displacementMapPath.c_str());
   pBaseElement->LinkEndChild(pMaterialElement);

   return pBaseElement;
}

std::shared_ptr<SceneNode> BoxRenderComponent::CreateSceneNode()
{
   if (m_pModelNode == nullptr)
      return std::shared_ptr<SceneNode>();

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pModelNode->SetTransform(pTransformComponent);
   }

   return m_pModelNode;
}

/******************************************/
/***********ModelRenderComponent***********/
/******************************************/

bool ModelRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pModelNode == nullptr) {
      m_pModelNode = std::make_shared<ModelNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
   }

   tinyxml2::XMLElement* pModel = pData->FirstChildElement("Model");
   if (!pModel) {
      Logger::WriteLog(Logger::LogType::ERROR, "Erro while loading actor" + std::to_string(m_pOwner->GetId()) + "; ModelRenderComponent must have path for model loading;");
      m_pModelNode.reset();
      return false;
   }

   const char* modelPath;
   pModel->QueryStringAttribute("path", &modelPath);
   m_modelPath = modelPath;

   auto modelData = std::static_pointer_cast<ModelData>(ResCache::Get()->GetHandle(m_modelPath)->GetExtra());

   if (modelData == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading actor" + std::to_string(m_pOwner->GetId()) + "; Error while loading model in ModelRenderComponent;");
      m_pModelNode.reset();
      return false;
   }

   m_pModelNode->SetModel(modelData->GetModel());

   return true;
}

tinyxml2::XMLElement* ModelRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Model");
   pMaterialElement->SetAttribute("path", m_modelPath.c_str());
   pBaseElement->LinkEndChild(pMaterialElement);

   return pBaseElement;
}

std::shared_ptr<SceneNode> ModelRenderComponent::CreateSceneNode()
{
   if (m_pModelNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pModelNode->SetTransform(pTransformComponent);
   }

   return m_pModelNode;
}

/******************************************/
/***********SkeletalModelRenderComponent***********/
/******************************************/

bool SkeletalModelRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!BaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pModelNode == nullptr) {
      m_pModelNode = std::make_shared<SkeletalModelNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
   }

   tinyxml2::XMLElement* pModel = pData->FirstChildElement("Model");
   if (!pModel) {
      Logger::WriteLog(Logger::LogType::ERROR, "Erro while loading actor" + std::to_string(m_pOwner->GetId()) + "; ModelRenderComponent must have path for model loading;");
      m_pModelNode.reset();
      return false;
   }

   const char* modelPath;
   pModel->QueryStringAttribute("path", &modelPath);
   m_modelPath = modelPath;

   auto modelData = std::static_pointer_cast<SkeletalModelData>(ResCache::Get()->GetHandle(m_modelPath)->GetExtra());

   if (modelData == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading actor" + std::to_string(m_pOwner->GetId()) + "; Error while loading model in ModelRenderComponent;");
      m_pModelNode.reset();
      return false;
   }

   m_pModelNode->SetSkeletalModel(modelData->GetSkeletalModel());

   return true;
}

tinyxml2::XMLElement* SkeletalModelRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Model");
   pMaterialElement->SetAttribute("path", m_modelPath.c_str());
   pBaseElement->LinkEndChild(pMaterialElement);

   return pBaseElement;
}

std::shared_ptr<SceneNode> SkeletalModelRenderComponent::CreateSceneNode()
{
   if (m_pModelNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   if (pTransformComponent) {
      m_pModelNode->SetTransform(pTransformComponent);
   }

   return m_pModelNode;
}

} // namespace BIEngine
