#pragma once

#include "ActorComponent.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/UniquePtr.h"
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
   SharedPtr<Sprite> m_pSprite;
   String m_spritePath;
   ColorRgba m_spriteColor;
};

static UniquePtr<ActorComponent> CreateSpriteRenderComponent()
{
   return MakeUnique<SpriteRenderComponent>();
}

class MeshBaseRenderComponent : public ActorComponent {
public:
   tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

protected:
   bool Init(tinyxml2::XMLElement* pData) override;

private:
   String m_materialPath;

protected:
   SharedPtr<Material> m_pMaterial;
};

class MeshRenderComponent : public MeshBaseRenderComponent {
public:
   SharedPtr<Model> GetModel() const { return m_pModel; }

   virtual void OnRenderObject(const GameTimer& gt) override;

protected:
   SharedPtr<Model> m_pModel;
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

static UniquePtr<ActorComponent> CreateBoxRenderComponent()
{
   return MakeUnique<BoxRenderComponent>();
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

static UniquePtr<ActorComponent> CreateSphereRenderComponent()
{
   return MakeUnique<SphereRenderComponent>();
}

class ModelRenderComponent : public MeshRenderComponent {
public:
   ModelRenderComponent()
      : MeshRenderComponent(), m_modelPath() {}

   static ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return ModelRenderComponent::g_CompId; }

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   SharedPtr<Model> GetModel() const { return m_pModel; }

protected:
   virtual bool Init(tinyxml2::XMLElement* pData);

protected:
   String m_modelPath;
};

static UniquePtr<ActorComponent> CreateModelRenderComponent()
{
   return MakeUnique<ModelRenderComponent>();
}

} // namespace BIEngine
