#pragma once

#include <memory>
#include <string>
#include <vector>
#include <list>

#include <glm/glm.hpp>

#include "../Graphics/Material.h"
#include "../Actors/TransformComponent.h"
#include "../Actors/Actor.h"

namespace BIEngine
{

	class Scene;
	class SceneNode;

	//Разные слои отображения отрисовываются в разном порядке
	enum class RenderLayer : int
	{
		BEGIN,
		OPAQUE = BEGIN,
		END
	};

	class SceneNodeProperties
	{
		friend class SceneNode;

	public:
		const ActorId& GetActorId() const { return m_ActorId; }


		void SetTransform(std::shared_ptr<TransformComponent> pTransform) { m_pTransformComponent = pTransform; }

		glm::vec3 GetPosition() const { return m_pTransformComponent->GetPosition(); }
		glm::vec3 GetSize() const { return m_pTransformComponent->GetSize(); };
		glm::vec3 GetRotation() const { return m_pTransformComponent->GetRotation(); }

		RenderLayer GetRenderLayer() const { return m_renderLayer; }

		bool Hasalpha() const { return m_pMaterial->HasAlpha(); }
		virtual float Alpha() const { return m_pMaterial->GetAlpha(); }

		void SetMaterial(std::shared_ptr<Material> pMat) { m_pMaterial = pMat; }
		std::shared_ptr<Material> GetMaterial() const { return m_pMaterial; }

	protected:
		void SetAlpha(const float alpha) {
			m_pMaterial->SetAlpha(alpha);
		}

		void SetRenderLayer(RenderLayer renderLayer) {
			m_renderLayer = renderLayer;
		}

	protected:
		ActorId m_ActorId;

		//Указатель на компонент, содержащий пространственные данные для актера, которые помогают спозицианировать объект при отрисовке
		std::shared_ptr<TransformComponent> m_pTransformComponent;

		RenderLayer m_renderLayer;

		std::shared_ptr<Material> m_pMaterial;
	};

	//Основной класс узла дерева спрайтов. 
	//Древовидная структура нужна для того, чтобы мы могли позиционировать одни графиечские элементы относительно других, взяв их координаты как за точку отсчета
	class ISceneNode
	{
	public:
		virtual ~ISceneNode() {};

		virtual const SceneNodeProperties* const Get() const = 0;

		virtual void SetTransform(std::shared_ptr<TransformComponent> pTransform) = 0;
		virtual glm::vec3 GetPosition() const = 0;
		virtual glm::vec3 GetSize() const = 0;
		virtual glm::vec3 GetRotation() const = 0;

		virtual bool OnUpdate(Scene* pScene, float dt) = 0;
		virtual bool PreRender(Scene* pScene) = 0;
		virtual bool OnRender(Scene* pScene) = 0;
		virtual bool RenderChildren(Scene* pScene) = 0;
		virtual bool PostRender(Scene* pScene) = 0;
		virtual bool IsVisible(Scene* pScene) const = 0;

		virtual bool AddChild(std::shared_ptr<ISceneNode> pChild) = 0;
		virtual bool RemoveChild(ActorId id) = 0;
	};


	typedef std::vector<std::shared_ptr<ISceneNode>> SceneNodeList;

	class SceneNode : public ISceneNode
	{
		friend class Scene;
	public:
		SceneNode(ActorId actorId,
			RenderLayer renderLayer)
			: m_pParent(nullptr)
		{
			m_props.m_ActorId = actorId;
			m_props.SetRenderLayer(renderLayer);
		}

		virtual ~SceneNode() {};
		virtual const SceneNodeProperties* const Get() const { return &m_props; }

		virtual void SetTransform(std::shared_ptr<TransformComponent> pTransform) { m_props.SetTransform(pTransform); }
		virtual glm::vec3 GetPosition() const { return  m_props.GetPosition(); }
		virtual glm::vec3 GetSize() const { return  m_props.GetSize(); };
		virtual glm::vec3 GetRotation() const { return  m_props.GetRotation(); }
		virtual glm::mat4 GetLocalModelMaatrix() const;

		virtual bool OnUpdate(Scene* pScene, float dt);
		//Если данное событие вернут "false", то отрисовка объекта в данном кадре будет отменена, но событие PostRender будет выполнено
		virtual bool PreRender(Scene* pScene);
		virtual bool OnRender(Scene* pScene) { return 0; }
		virtual bool RenderChildren(Scene* pScene);
		virtual bool PostRender(Scene* pScene);
		virtual bool IsVisible(Scene* pScene) const { return true; };

		virtual bool AddChild(std::shared_ptr<ISceneNode> pChild);
		virtual bool RemoveChild(ActorId id);

		void SetRenderLayer(RenderLayer renderLayer) { m_props.SetRenderLayer(renderLayer); }
		RenderLayer GetRenderLayer() const { return m_props.GetRenderLayer(); }

		void SetMaterial(std::shared_ptr<Material> pMat) { m_props.SetMaterial(pMat); }

	protected:
		SceneNodeList m_children;
		SceneNode* m_pParent;
		SceneNodeProperties m_props;
	};

	//Корневой узел сцены, с которого начинается каждая отрисовка сцены
	class RootNode : public SceneNode
	{
	public:
		RootNode();
		virtual bool AddChild(std::shared_ptr<ISceneNode> pChild);
		virtual bool RenderChildren(Scene* pScene);

		//В стандартной имплементации, мы во время pre и post обработки сохраняем относительные координаты для renderera. У рута нет координат - значит не надо пушить данные в стэк
		virtual bool PreRender(Scene* pScene) override { return true; }
		virtual bool PostRender(Scene* pScene) override { return true; }
	};


	class CameraNode : public SceneNode
	{
	public:
		enum class ProjectionType
		{
			ORTHO,
			PERSPECTIVE
		};

		CameraNode()
			: SceneNode(Actor::INVALID_ACTOR_ID, RenderLayer::BEGIN)
			, m_projType(ProjectionType::ORTHO)
		{
		}

		void SetProjType(ProjectionType type) { m_projType = type; }

		//Здесь происходит обсчет матриц, которые отправляются в Renderer, чтобы задать позицию камеры
		virtual bool OnRender(Scene* pScene) override;
		//Всегда возвращает "true", так как для вызова OnRender требуется, чтобы объект был видимым
		virtual bool IsVisible(Scene* pScene) const { return true; }

	private:
		ProjectionType m_projType;
	};
}