#include "Scene.h"

#include "../Actors/CameraComponent.h"

namespace BIEngine
{

	Scene::Scene(std::shared_ptr<Renderer> pRenderer)
		: m_pRenderer(pRenderer)
		, m_pCamera(nullptr)
	{
		m_pRoot = std::make_shared<RootNode>();

		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Scene::NewCameraComponentDelegate), EvtData_New_Camera_Component::sk_EventType);
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);

		m_coordStack.push({ glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), 0 });
	}

	Scene::~Scene()
	{
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::NewCameraComponentDelegate), EvtData_New_Camera_Component::sk_EventType);
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	}

	int Scene::OnRender() 
	{
		if (m_pRoot && m_pCamera) 
		{
			//Камера задает значения для всех необходимых матриц
			m_pCamera->OnRender(this);

			if (m_pRoot->PreRender(this)) 
			{
				m_pRoot->OnRender(this);
				m_pRoot->RenderChildren(this);
			}
			m_pRoot->PostRender(this);
		}

		return 0;
	}

	int Scene::OnUpdate(float dt) {
		if (!m_pRoot)
			return 0;

		return m_pRoot->OnUpdate(this, dt);
	}

	void Scene::PushMatrix(const glm::vec2& position, const glm::vec2& size, float angle)
	{
		auto topCoords = m_coordStack.top();
		glm::vec2 rotatedCoords = std::get<0>(topCoords);

		//Получаем новый угол на основе сложения текущего угла с углом родителя
		float newAngle = std::get<2>(topCoords) + angle;


		//TODO: мы не проверяем, какой нам угол пришел, если будет угол больше, чем |360|, то эта проверка не поможет
		if (newAngle > 360.0)
			newAngle -= 360.0;

		if (newAngle < -360.0)
			newAngle += 360.0;


		//Переводим наш старый угол в раидана и высчитываем синусы/косинусы
		const float pi = 3.141592;
		const float radians = (std::get<2>(topCoords) * (pi / 180));

		const float sinOfAng = std::sin(radians);
		const float cosOfAng = std::cos(radians);

		//Получаем координаты объекта в новом базисе
		rotatedCoords.x += position.x * cosOfAng - position.y * sinOfAng;
		rotatedCoords.y += position.x * sinOfAng + position.y * cosOfAng;

		m_coordStack.push({ rotatedCoords, size, newAngle });
		m_pRenderer->SetModelTransform(rotatedCoords, size, newAngle);
	}

	void Scene::PopMatrix()
	{
		m_coordStack.pop();
		auto topCoords = m_coordStack.top();
		m_pRenderer->SetModelTransform(std::get<0>(topCoords), std::get<1>(topCoords), std::get<2>(topCoords));
	}

	std::shared_ptr<ISceneNode> Scene::FindActor(ActorId id) {
		auto itr = m_actorMap.find(id);
		if (itr == m_actorMap.end())
			return std::shared_ptr<ISceneNode>();

		return (*itr).second;
	}

	bool Scene::AddChild(ActorId id, std::shared_ptr<ISceneNode> pChild) {
		m_actorMap[id] = pChild;

		return m_pRoot->AddChild(pChild);
	}

	bool Scene::RemoveChild(ActorId id)
	{
		std::shared_ptr<ISceneNode> pChild = FindActor(id);

		m_actorMap.erase(id);
		return m_pRoot->RemoveChild(id);
	}

	//Вызывается, когда создается новый актер с графическим компонентом
	void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_New_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Render_Component>(pEventData);

		ActorId actorId = pCastEventData->GetActorId();
		std::shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());

		AddChild(actorId, pSceneNode);
	}

	//Вызывается, когда создается новый актер с компонентой-камерой.
	void Scene::NewCameraComponentDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_New_Camera_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Camera_Component>(pEventData);

		auto pCameraActor = pCastEventData->GetCameraActor();

		std::shared_ptr<CameraComponent> pCameraComponent = pCameraActor->GetComponent<CameraComponent>(CameraComponent::g_CompId).lock();
		if (pCameraComponent)
		{
			m_pCamera = pCameraComponent->GetCameraNode();
		}
	}

	void Scene::DestroyActorDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
		RemoveChild(pCastEventData->GetId());
	}
}