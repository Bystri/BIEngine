#include "SceneNodes.h"

#include "../Graphics2D/Scene.h"

namespace BIEngine
{

	bool SceneNode::OnUpdate(Scene* pScene, float dt) {
		for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
			(*itr)->OnUpdate(pScene, dt);
		}

		return true;
	}

	bool SceneNode::PreRender(Scene* pScene)
	{
		return true;
	}

	bool SceneNode::PostRender(Scene* pScene)
	{
		return true;
	}

	bool SceneNode::RenderChildren(Scene* pScene) 
	{
		for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) 
		{
			if ((*itr)->PreRender(pScene)) {
				if ((*itr)->IsVisible(pScene)) 
				{
					if ((*itr)->Get()->GetRenderLayer() == RenderLayer::OPAQUE)
						(*itr)->OnRender(pScene);
				}

				(*itr)->RenderChildren(pScene);
			}
			(*itr)->PostRender(pScene);
		}

		return true;
	}

	bool SceneNode::AddChild(std::shared_ptr<ISceneNode> pChild) {
		m_children.push_back(pChild);

		//TODO: Что насчет, если наш ребенок будет БОЛЬШЕ родителя? Что насчет удаления?

		return true;
	}

	bool SceneNode::RemoveChild(ActorId id) 
	{
		for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
		{
			const SceneNodeProperties* pProps = (*itr)->Get();

			if ((*itr)->RemoveChild(id))
				return true;

			if (id == pProps->GetActorId())
			{
				itr = m_children.erase(itr);
				return true;
			}
		}
		return false;
	}


	RootNode::RootNode()
		:SceneNode(Actor::INBALID_ACTOR_ID, RenderLayer::BEGIN)
	{
		m_children.reserve(static_cast<size_t>(RenderLayer::END));

		m_children.push_back(std::make_shared<SceneNode>(Actor::INBALID_ACTOR_ID, RenderLayer::OPAQUE));
	}

	bool RootNode::AddChild(std::shared_ptr<ISceneNode> pChild) 
	{
		RenderLayer pass = pChild->Get()->GetRenderLayer();
		if (static_cast<unsigned>(pass) >= m_children.size() || !m_children[static_cast<std::size_t>(pass)])
		{
			assert(0 && "There is no such render pass");
			return false;
		}

		return m_children[static_cast<std::size_t>(pass)]->AddChild(pChild);
	}

	bool RootNode::RenderChildren(Scene* pScene) 
	{

		for (int pass = static_cast<unsigned>(RenderLayer::BEGIN); pass < static_cast<unsigned>(RenderLayer::END); ++pass) {
			switch (static_cast<RenderLayer>(pass)) 
			{
			case RenderLayer::BEGIN:
				m_children[pass]->RenderChildren(pScene);
			}
		}

		return true;
	}

	/***CameraNode***/
	bool CameraNode::OnRender(Scene* pScene)
	{
		//Берем позицию камеры из свойств
		glm::vec3 cameraPosition = glm::vec3(m_props.GetPosition().x, m_props.GetPosition().y, 1);

		//Далее мы находим направлением камеры
		glm::vec3 cameraTarget = cameraPosition;
		cameraPosition.z = 0; //Камера стоит на позиции z = 1, а смотрим мы в z = 0
		glm::vec3 cameraReverseDir = glm::normalize(cameraPosition - cameraTarget);

		//Правая ось
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraReverseDir));

		//Верхняя ось
		glm::vec3 cameraUp = glm::cross(cameraReverseDir, cameraRight);

		glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

		pScene->GetRenderer()->SetViewTransform(view);
		return true;
	}

}