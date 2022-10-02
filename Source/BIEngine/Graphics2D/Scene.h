#pragma once

#include <map>
#include <memory>

#include "SceneNodes.h"
#include "Renderer.h"
#include "../EventManager/Events.h"

namespace BIEngine
{

	typedef std::map<ActorId, std::shared_ptr<ISceneNode>> SceneActorMap;

	//Владеет всеми графическими элементами, необходимым для отрисовки
	class Scene
	{
	public:
		Scene(std::shared_ptr<Renderer> pRenderer);
		virtual ~Scene();

		int OnRender();
		int OnUpdate(float dt);

		std::shared_ptr<ISceneNode> FindActor(ActorId id);

		void SetCamera(std::shared_ptr<CameraNode> pCamera) { m_pCamera = pCamera; }
		const std::shared_ptr<CameraNode> GetCamera() const { return m_pCamera; }

		bool AddChild(ActorId id, std::shared_ptr<ISceneNode> pChild);
		bool RemoveChild(ActorId id);

		std::shared_ptr<Renderer> GetRenderer() { return m_pRenderer; }

	protected:
		//Регистрация компонента, который необохдимо отрисовать
		void NewRenderComponentDelegate(IEventDataPtr pEventData);
		//Регистрация камеры, являющейся компонентом актера. Каждая новая созданная камера перезаписывает старую.
		void NewCameraComponentDelegate(IEventDataPtr pEventData);
		//Удаление уничтоженного актера из сцены
		void DestroyActorDelegate(IEventDataPtr pEventData);

	protected:
		//Основной узел дерева графических элементов
		std::shared_ptr<SceneNode> m_pRoot;

		std::shared_ptr<CameraNode> m_pCamera;
		//Рисовальщик
		std::shared_ptr<Renderer> m_pRenderer;

		SceneActorMap m_actorMap;
	};

}