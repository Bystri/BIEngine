#pragma once

#include <memory>
#include <string>
#include <vector>
#include <list>

#include <glm/glm.hpp>

#include "../Actors/Actor.h"
#include "../Actors/TransformComponent.h"
#include "../Graphics/Material.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class Scene;
class SceneNode;

// Разные слои отображения отрисовываются в разном порядке
enum class RenderLayer : int {
   BEGIN,
   LIGHT = BEGIN,
   OPAQUE,
   END
};

class SceneNodeProperties {
   friend class SceneNode;

public:
   const ActorId& GetActorId() const { return m_ActorId; }

   void SetTransform(std::shared_ptr<TransformComponent> pTransform) { m_pTransformComponent = pTransform; }

   glm::vec3 GetPosition() const { return m_pTransformComponent->GetPosition(); }

   glm::vec3 GetSize() const { return m_pTransformComponent->GetSize(); };

   glm::vec3 GetRotation() const { return m_pTransformComponent->GetRotation(); }

   glm::mat4 GetTransformMatrix() const { return m_pTransformComponent->GetTransformMatrix(); }

   RenderLayer GetRenderLayer() const { return m_renderLayer; }

protected:
   void SetRenderLayer(RenderLayer renderLayer)
   {
      m_renderLayer = renderLayer;
   }

protected:
   ActorId m_ActorId;

   // Указатель на компонент, содержащий пространственные данные для актера, которые помогают спозицианировать объект при отрисовке
   std::shared_ptr<TransformComponent> m_pTransformComponent;

   RenderLayer m_renderLayer;
};

// Основной класс узла дерева спрайтов.
// Древовидная структура нужна для того, чтобы мы могли позиционировать одни графиечские элементы относительно других, взяв их координаты как за точку отсчета
class ISceneNode {
public:
   virtual ~ISceneNode(){};

   virtual const SceneNodeProperties* const Get() const = 0;

   virtual void SetTransform(std::shared_ptr<TransformComponent> pTransform) = 0;
   virtual glm::vec3 GetPosition() const = 0;
   virtual glm::vec3 GetSize() const = 0;
   virtual glm::vec3 GetRotation() const = 0;

   virtual bool OnUpdate(Scene* pScene, const GameTimer& gt) = 0;
   virtual bool PreRender(Scene* pScene) = 0;
   virtual bool OnRender(Scene* pScene) = 0;
   virtual bool RenderChildren(Scene* pScene) = 0;
   virtual bool PostRender(Scene* pScene) = 0;
   virtual bool IsVisible(Scene* pScene) const = 0;

   virtual void AddChild(std::shared_ptr<ISceneNode> pChild) = 0;
   virtual void RemoveChild(ActorId id) = 0;
};

using SceneNodeList = std::vector<std::shared_ptr<ISceneNode>>;

class SceneNode : public ISceneNode {
   friend class Scene;

public:
   SceneNode(ActorId actorId, RenderLayer renderLayer)
      : m_pParent(nullptr)
   {
      m_props.m_ActorId = actorId;
      m_props.SetRenderLayer(renderLayer);
   }

   virtual ~SceneNode(){};

   virtual const SceneNodeProperties* const Get() const { return &m_props; }

   virtual void SetTransform(std::shared_ptr<TransformComponent> pTransform) { m_props.SetTransform(pTransform); }

   virtual glm::vec3 GetPosition() const { return m_props.GetPosition(); }

   virtual glm::vec3 GetSize() const { return m_props.GetSize(); };

   virtual glm::vec3 GetRotation() const { return m_props.GetRotation(); }

   virtual glm::mat4 GetLocalModelMatrix() const;

   virtual bool OnUpdate(Scene* pScene, const GameTimer& gt);
   // Если данное событие вернут "false", то отрисовка объекта в данном кадре будет отменена, но событие PostRender будет выполнено
   virtual bool PreRender(Scene* pScene);

   virtual bool OnRender(Scene* pScene) { return 0; }

   virtual bool RenderChildren(Scene* pScene);
   virtual bool PostRender(Scene* pScene);

   virtual bool IsVisible(Scene* pScene) const { return true; };

   virtual void AddChild(std::shared_ptr<ISceneNode> pChild) override;
   virtual void RemoveChild(ActorId id) override;

   void SetRenderLayer(RenderLayer renderLayer) { m_props.SetRenderLayer(renderLayer); }

   RenderLayer GetRenderLayer() const { return m_props.GetRenderLayer(); }

protected:
   SceneNodeList m_children;
   SceneNode* m_pParent;
   SceneNodeProperties m_props;
};

// Корневой узел сцены, с которого начинается каждая отрисовка сцены
class RootNode : public SceneNode {
public:
   RootNode();
   virtual ~RootNode();

   virtual void AddChild(std::shared_ptr<ISceneNode> pChild) override;
   virtual bool RenderChildren(Scene* pScene) override;

   // В стандартной имплементации, мы во время pre и post обработки сохраняем относительные координаты для renderera. У рута нет координат - значит не надо пушить данные в стэк
   virtual bool PreRender(Scene* pScene) override { return true; }

   virtual bool PostRender(Scene* pScene) override { return true; }
};

class DirectionalLightNode : public SceneNode {
public:
   explicit DirectionalLightNode(const ActorId actorId)
      : SceneNode(actorId, RenderLayer::LIGHT)
   {
   }

   virtual ~DirectionalLightNode() {}

   void SetIrradiance(float irradiance) { m_irradiance = irradiance; }

   float GetIrradiance() const { return m_irradiance; }

   void SetColor(const ColorRgb& color) { m_color = color; }

   const ColorRgb& GetColor() const { return m_color; }

   virtual bool OnRender(Scene* pScene);

private:
   glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);
   float m_irradiance = 1.0f;
   ColorRgb m_color = ColorRgb(0.75f, 0.75f, 0.05f);
};

class PointLightNode : public SceneNode {
public:
   explicit PointLightNode(const ActorId actorId)
      : SceneNode(actorId, RenderLayer::LIGHT)
   {
   }

   virtual ~PointLightNode() {}

   void SetIntensity(float intensity) { m_intensity = intensity; }

   float GetIntensity() const { return m_intensity; }

   void SetColor(const ColorRgb& color) { m_color = color; }

   const ColorRgb& GetColor() const { return m_color; }

   virtual bool OnRender(Scene* pScene);

private:
   float m_intensity = 1.0f;
   ColorRgb m_color = ColorRgb(0.75f, 0.75f, 0.05f);
};

class SpotLightNode : public SceneNode {
public:
   explicit SpotLightNode(const ActorId actorId)
      : SceneNode(actorId, RenderLayer::LIGHT)
   {
   }

   virtual ~SpotLightNode() {}

   void SetAttenuationParams(float constant, float linear, float quadratic)
   {
      m_attenuationConstant = constant;
      m_attenuationLinear = linear;
      m_attenuationQuadratic = quadratic;
   }

   float GetAttenuationConstant() const { return m_attenuationConstant; }

   float GetAttenuationLinear() const { return m_attenuationLinear; }

   float GetAttenuationQuadratic() const { return m_attenuationQuadratic; }

   void SetCutOff(float cutOff) { m_cutOff = cutOff; }

   float GetCutOff() const { return m_cutOff; }

   void SetOuterCutOff(float outerCutOff) { m_outerCutOff = outerCutOff; }

   float GetOuterCutOff() const { return m_outerCutOff; }

   void SetAmbient(const ColorRgb& ambient) { m_ambient = ambient; }

   const ColorRgb& GetAmbient() const { return m_ambient; }

   void SetDiffuse(const ColorRgb& diffuse) { m_diffuse = diffuse; }

   const ColorRgb& GetDiffuse() const { return m_diffuse; }

   void SetSpecular(const ColorRgb& specular) { m_specular = specular; }

   const ColorRgb& GetSpecular() const { return m_specular; }

   virtual bool OnRender(Scene* pScene);

private:
   float m_attenuationConstant = 1.0f;
   float m_attenuationLinear = 0.09f;
   float m_attenuationQuadratic = 0.032f;

   ColorRgb m_ambient = ColorRgb(0.05f, 0.05f, 0.05f);
   ColorRgb m_diffuse = ColorRgb(0.5f, 0.5f, 0.5f);
   ColorRgb m_specular = ColorRgb(1.0f, 1.0f, 1.0f);

   float m_cutOff = glm::cos(glm::radians(12.5f));
   float m_outerCutOff = glm::cos(glm::radians(25.0f));
};

class Model;

class ModelNode : public SceneNode {
public:
   ModelNode(const ActorId actorId, RenderLayer renderLayer)
      : SceneNode(actorId, renderLayer), m_pModel(nullptr)
   {
   }

   virtual ~ModelNode() {}

   void SetModel(std::shared_ptr<Model> pModel) { m_pModel = pModel; }

   std::shared_ptr<Model> GetModel() const { return m_pModel; }

   virtual bool OnRender(Scene* pScene);

protected:
   std::shared_ptr<Model> m_pModel;
};

class SkeletalModel;

class SkeletalModelNode : public SceneNode {
public:
   SkeletalModelNode(const ActorId actorId, RenderLayer renderLayer)
      : SceneNode(actorId, renderLayer), m_pModel(nullptr)
   {
   }

   virtual ~SkeletalModelNode() {}

   void SetSkeletalModel(std::shared_ptr<SkeletalModel> pModel) { m_pModel = pModel; }

   std::shared_ptr<SkeletalModel> GetModel() const { return m_pModel; }

   virtual bool OnRender(Scene* pScene);

protected:
   std::shared_ptr<SkeletalModel> m_pModel;
};

} // namespace BIEngine
