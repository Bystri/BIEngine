#include "Scene.h"


namespace BIEngine
{

	Scene::Scene(std::shared_ptr<Renderer> pRenderer)
		: m_pRenderer(pRenderer)
		, m_pRoot(std::make_shared<RootNode>())
		, m_pConstantsBuffer(std::make_shared<ConstantsBuffer>())
		, m_pCamera(nullptr)
		, m_pSkybox(nullptr)
	{
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	}


	Scene::~Scene()
	{
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	}


	void Scene::Init()
	{
		constexpr int CONSTANTS_BUFFER_SCENE_GLOBALS_BINDING_POINT = 0;

		m_pConstantsBuffer->Init(sizeof(GlobalRenderBufferData), CONSTANTS_BUFFER_SCENE_GLOBALS_BINDING_POINT);
	}


	int Scene::OnRender() 
	{
		if (m_pRoot && m_pCamera) 
		{
			static constexpr Color CLEAR_COLOR = Color(0.0f, 0.5f, 0.5f, 1.0f);
			m_pRenderer->Clear(RenderDevice::ClearFlag::COLOR | RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

			m_globalRenderBufferData.viewMat = m_pCamera->GetViewMatrix();
			m_globalRenderBufferData.projMat = m_pCamera->GetProjMatrix();
			m_pConstantsBuffer->SetBufferData(&m_globalRenderBufferData, 0, sizeof(m_globalRenderBufferData));

			if (m_pRoot->PreRender(this)) 
			{
				m_pRoot->OnRender(this);
				m_pRoot->RenderChildren(this);
			}
			m_pRoot->PostRender(this);

			if (m_pSkybox) {
				m_pSkybox->OnRender(this);
			}
		}

		return 0;
	}


	int Scene::OnUpdate(float dt) {
		if (!m_pRoot)
			return 0;

		return m_pRoot->OnUpdate(this, dt);
	}


	std::shared_ptr<ISceneNode> Scene::FindActor(ActorId id) {
		auto itr = m_actorMap.find(id);
		if (itr == m_actorMap.end())
			return std::shared_ptr<ISceneNode>();

		return (*itr).second;
	}


	void Scene::AddChild(ActorId id, std::shared_ptr<ISceneNode> pChild) {
		m_actorMap[id] = pChild;

		m_pRoot->AddChild(pChild);
	}
	

	void Scene::RemoveChild(ActorId id)
	{
		std::shared_ptr<ISceneNode> pChild = FindActor(id);

		m_actorMap.erase(id);
		m_pRoot->RemoveChild(id);
	}


	//Вызывается, когда создается новый актер с графическим компонентом
	void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_New_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Render_Component>(pEventData);

		ActorId actorId = pCastEventData->GetActorId();
		std::shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());

		AddChild(actorId, pSceneNode);
	}


	void Scene::DestroyActorDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
		RemoveChild(pCastEventData->GetId());
	}
}