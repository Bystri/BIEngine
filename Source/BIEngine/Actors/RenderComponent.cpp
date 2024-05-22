#include "RenderComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Renderer/ShadersLoader.h"
#include "../Graphics/ModelLoader.h"
#include "../Graphics/MaterialLoader.h"
#include "../Graphics/SkeletalModelLoader.h"
#include "../Renderer/MeshGeometryGenerator.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"
#include "../Renderer/TextureLoader.h"

namespace BIEngine {
ComponentId SpriteRenderComponent::g_CompId = "SpriteRenderComponent";
ComponentId BoxRenderComponent::g_CompId = "BoxRenderComponent";
ComponentId SphereRenderComponent::g_CompId = "SphereRenderComponent";
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
   if (!m_pSceneNode) {
      m_pSceneNode = CreateSceneNode();
   }
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

   tinyxml2::XMLElement* pMaterialElement = pData->FirstChildElement("Material");

   if (pMaterialElement == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading RenderComponent for Actor with id: " + std::to_string(m_pOwner->GetId()) + "; No Material was specified");
      return false;
   }

   const char* materialPath;
   pMaterialElement->QueryStringAttribute("path", &materialPath);

   m_materialPath = materialPath;
   std::shared_ptr<MaterialData> pMaterialData = std::static_pointer_cast<MaterialData>(ResCache::Get()->GetHandle(m_materialPath)->GetExtra());
   m_pMaterial = pMaterialData->GetMaterial();

   return true;
}

tinyxml2::XMLElement* MeshBaseRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("path", m_materialPath.c_str());
   pBaseElement->LinkEndChild(pMaterialElement);

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

   static const std::string SPRITE_SHADER_PROGRAM_PATH = "Effects/sprite.bisp";

   std::shared_ptr<ShaderProgramData> pShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(SPRITE_SHADER_PROGRAM_PATH)->GetExtra());
   std::shared_ptr<Material> pMaterial = std::make_shared<Material>(pShaderProgramData->GetShaderProgram());

   m_spriteColor = COLOR_WHITE;

   tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
   if (pColorElement) {
      float r = 0;
      float g = 0;
      float b = 0;
      pColorElement->QueryFloatAttribute("r", &r);
      pColorElement->QueryFloatAttribute("g", &g);
      pColorElement->QueryFloatAttribute("b", &b);
      m_spriteColor = ColorRgba(r, g, b, 1.0f);
   }

   pMaterial->SetColorRgba("material.color", m_spriteColor);

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

      pMaterial->AddTexture("material.sprite", 0, spriteData->GetTexture());

      std::shared_ptr<Sprite> pSprite = std::make_shared<Sprite>(pMaterial);
      m_pSpriteNode->SetSprite(pSprite);
   }

   return true;
}

tinyxml2::XMLElement* SpriteRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
   pColor->SetAttribute("r", std::to_string(m_spriteColor.r).c_str());
   pColor->SetAttribute("g", std::to_string(m_spriteColor.g).c_str());
   pColor->SetAttribute("b", std::to_string(m_spriteColor.b).c_str());
   pBaseElement->LinkEndChild(pColor);


   tinyxml2::XMLElement* pSprite = pDoc->NewElement("Sprite");
   pSprite->SetAttribute("path", m_spritePath.c_str());
   pBaseElement->LinkEndChild(pSprite);

   return pBaseElement;
}

std::shared_ptr<SceneNode> SpriteRenderComponent::CreateSceneNode()
{
   if (m_pSpriteNode == nullptr) {
      return std::shared_ptr<SceneNode>();
   }

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

   std::shared_ptr<Mesh> boxMesh = std::make_shared<Mesh>(MeshGeometryGenerator::CreateBox(m_width, m_height, m_depth, 0u));
   std::shared_ptr<ModelMesh> pModelMesh = std::make_shared<ModelMesh>(boxMesh, m_pMaterial);

   std::shared_ptr<Model> pModel = std::make_shared<Model>();
   pModel->AddModelMesh(pModelMesh);
   m_pModelNode->SetModel(pModel);

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

   return pBaseElement;
}

std::shared_ptr<SceneNode> BoxRenderComponent::CreateSceneNode()
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

/***********************************************************
 * SphereRenderComponent
 ************************************************************/

bool SphereRenderComponent::Init(tinyxml2::XMLElement* pData)
{
   if (!MeshBaseRenderComponent::Init(pData)) {
      return false;
   }

   if (m_pModelNode == nullptr) {
      m_pModelNode = std::make_shared<ModelNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
   }

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
   if (pSizeElement) {
      double r;
      pSizeElement->QueryDoubleAttribute("r", &r);

      m_radius = (float)r;
   }

   std::shared_ptr<Mesh> boxMesh = std::make_shared<Mesh>(MeshGeometryGenerator::CreateSphere(m_radius, 16.0f, 16.0f));
   std::shared_ptr<ModelMesh> pModelMesh = std::make_shared<ModelMesh>(boxMesh, m_pMaterial);

   std::shared_ptr<Model> pModel = std::make_shared<Model>();
   pModel->AddModelMesh(pModelMesh);
   m_pModelNode->SetModel(pModel);

   return true;
}

tinyxml2::XMLElement* SphereRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = MeshBaseRenderComponent::GenerateXml(pDoc);

   tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
   pSize->SetAttribute("r", std::to_string(m_radius).c_str());
   pBaseElement->LinkEndChild(pSize);

   return pBaseElement;
}

std::shared_ptr<SceneNode> SphereRenderComponent::CreateSceneNode()
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
