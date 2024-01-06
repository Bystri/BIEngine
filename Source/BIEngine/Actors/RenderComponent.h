#pragma once

#include "ActorComponent.h"

#include "../Graphics/SpriteNode.h"
#include "../Graphics/LightReflectiveMaterial.h"
#include "../Renderer/Color.h"

namespace BIEngine {
// Данный компонент отвечает за регестрацию на сцене объекта отрисовки. Делается это с помощью отрпавки сообщения, которое прослушивает сама сцена
class BaseRenderComponent : public ActorComponent {
protected:
   std::shared_ptr<SceneNode> m_pSceneNode;

public:
   BaseRenderComponent() {}

   bool Init(tinyxml2::XMLElement* pData) override
   {
      assert(pData);

      return pData != nullptr;
   }

   tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override { return pDoc->NewElement(GetComponentId().c_str()); }

   virtual void PostInit() override;

protected:
   // В данном методе дочерние классы создают особые узлы сцены под свой заданный тип компонента актера
   virtual std::shared_ptr<SceneNode> CreateSceneNode() = 0;

private:
   virtual std::shared_ptr<SceneNode> GetSceneNode();
};

// Создает и регестрирует в сцене спрайт для отрисовки актера
class SpriteRenderComponent : public BaseRenderComponent {
public:
   SpriteRenderComponent()
      : BaseRenderComponent(), m_pSpriteNode(nullptr), m_spritePath() {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SpriteRenderComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode();

protected:
   std::shared_ptr<SpriteNode> m_pSpriteNode; // Узел на сцене, который отвечает за отрисовку этого компонента
   std::string m_spritePath;                  // Сохраняем путь к спрайту, из которого мы делаем текстуру, чтобы потом вставить его в XML, если понадобится
};

static ActorComponent* CreateSpriteRenderComponent()
{
   return new SpriteRenderComponent;
}

class MeshBaseRenderComponent : public BaseRenderComponent {
protected:
   bool Init(tinyxml2::XMLElement* pData) override;

   tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

private:
   std::string m_shaderProgrampath;

protected:
   std::shared_ptr<LightReflectiveMaterial> m_pLightReflectionMaterial;
};

class BoxRenderComponent : public MeshBaseRenderComponent {
public:
   BoxRenderComponent()
      : MeshBaseRenderComponent(), m_pModelNode(nullptr), m_width(1.0f), m_height(1.0f), m_depth(1.0f) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return BoxRenderComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode();

protected:
   std::shared_ptr<ModelNode> m_pModelNode; // Узел на сцене, который отвечает за отрисовку этого компонента

   std::string m_diffuseMapPath;
   std::string m_specularMapPath;
   std::string m_normalMapPath;

   float m_width;
   float m_height;
   float m_depth;
};

static ActorComponent* CreateBoxRenderComponent()
{
   return new BoxRenderComponent;
}

class ModelRenderComponent : public BaseRenderComponent {
public:
   ModelRenderComponent()
      : BaseRenderComponent(), m_pModelNode(nullptr), m_modelPath() {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return BoxRenderComponent::g_CompId; }

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual std::shared_ptr<SceneNode> CreateSceneNode();

protected:
   std::shared_ptr<ModelNode> m_pModelNode; // Узел на сцене, который отвечает за отрисовку этого компонента

   std::string m_modelPath;
};

static ActorComponent* CreateModelRenderComponent()
{
   return new ModelRenderComponent;
}
} // namespace BIEngine
