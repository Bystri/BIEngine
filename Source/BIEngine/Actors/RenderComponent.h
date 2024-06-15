#pragma once

#include "ActorComponent.h"

#include "../EngineCore/Assert.h"
#include "../Graphics/Sprite.h"
#include "../Renderer/Color.h"
#include "../Graphics/Model.h"

namespace BIEngine {
// Создает и регестрирует в сцене спрайт для отрисовки актера
class SpriteRenderComponent : public ActorComponent {
public:
   SpriteRenderComponent()
      : ActorComponent(), m_spritePath(), m_spriteColor(COLOR_WHITE) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SpriteRenderComponent::g_CompId; }

   virtual void OnRenderObject(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData);

protected:
   std::shared_ptr<Sprite> m_pSprite;
   std::string m_spritePath;
   ColorRgba m_spriteColor;
};

static std::unique_ptr<ActorComponent> CreateSpriteRenderComponent()
{
   return std::make_unique<SpriteRenderComponent>();
}

class MeshBaseRenderComponent : public ActorComponent {
public:
   tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   bool Init(tinyxml2::XMLElement* pData) override;

private:
   std::string m_materialPath;

protected:
   std::shared_ptr<Material> m_pMaterial;
};

class MeshRenderComponent : public MeshBaseRenderComponent {
public:
   std::shared_ptr<Model> GetModel() const { return m_pModel; }

   virtual void OnRenderObject(const GameTimer& gt) override;

protected:
   std::shared_ptr<Model> m_pModel;
};

class ModelMesh;

class BoxRenderComponent : public MeshRenderComponent {
public:
   BoxRenderComponent()
      : MeshRenderComponent(), m_width(1.0f), m_height(1.0f), m_depth(1.0f) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return BoxRenderComponent::g_CompId; }

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData);

protected:
   float m_width;
   float m_height;
   float m_depth;
};

static std::unique_ptr<ActorComponent> CreateBoxRenderComponent()
{
   return std::make_unique<BoxRenderComponent>();
}

class SphereRenderComponent : public MeshRenderComponent {
public:
   SphereRenderComponent()
      : MeshRenderComponent(), m_radius(1.0f) {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return SphereRenderComponent::g_CompId; }

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   virtual bool Init(tinyxml2::XMLElement* pData);

protected:
   float m_radius;
};

static std::unique_ptr<ActorComponent> CreateSphereRenderComponent()
{
   return std::make_unique<SphereRenderComponent>();
}

class ModelRenderComponent : public MeshRenderComponent {
public:
   ModelRenderComponent()
      : MeshRenderComponent(), m_modelPath() {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return ModelRenderComponent::g_CompId; }

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   std::shared_ptr<Model> GetModel() const { return m_pModel; }

protected:
   virtual bool Init(tinyxml2::XMLElement* pData);

protected:
   std::string m_modelPath;
};

static std::unique_ptr<ActorComponent> CreateModelRenderComponent()
{
   return std::make_unique<ModelRenderComponent>();
}

} // namespace BIEngine
