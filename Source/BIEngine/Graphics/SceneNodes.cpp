#include "SceneNodes.h"

#include "../Graphics/Scene.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine
{

	glm::mat4 SceneNode::GetLocalModelMatrix() const
	{
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_props.GetRotation().x),
			glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_props.GetRotation().y),
			glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_props.GetRotation().z),
			glm::vec3(0.0f, 0.0f, 1.0f));

		// Y * X * Z
		const glm::mat4 roationMatrix = transformY * transformX * transformZ;

		// translation * rotation * scale 
		auto ans = glm::translate(glm::mat4(1.0f), m_props.GetPosition()) *
			roationMatrix *
			glm::scale(glm::mat4(1.0f), m_props.GetSize());

		return m_props.GetTransformMatrix();
	}

	bool SceneNode::OnUpdate(Scene* pScene, float dt) {
		for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
			(*itr)->OnUpdate(pScene, dt);
		}

		return true;
	}

	bool SceneNode::PreRender(Scene* pScene)
	{
		pScene->PushMatrix(GetLocalModelMatrix());
		return true;
	}

	bool SceneNode::PostRender(Scene* pScene)
	{
		pScene->PopMatrix();
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
				m_children.erase(itr);
				return true;
			}
		}
		return false;
	}


	RootNode::RootNode()
		:SceneNode(Actor::INVALID_ACTOR_ID, RenderLayer::BEGIN)
	{
		m_children.reserve(static_cast<size_t>(RenderLayer::END));

		m_children.push_back(std::make_shared<SceneNode>(Actor::INVALID_ACTOR_ID, RenderLayer::OPAQUE));
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
		glm::mat4 proj;

		if (m_projType == ProjectionType::ORTHO)
			proj = glm::ortho(0.0f,	(float)g_pApp->m_options.screenWidth, (float)g_pApp->m_options.screenHeight, 0.0f, -1.0f, 1.0f);
		else
			proj = glm::perspective(glm::radians(45.0f), (float)g_pApp->m_options.screenWidth / (float)g_pApp->m_options.screenHeight, 0.1f, 1000.0f);

		pScene->GetRenderer()->SetProjection(proj);

		//Вектор направления взгляда камеры
		glm::vec3 front;
		const float pitch = m_props.GetRotation().x;
		const float yaw = m_props.GetRotation().y;
		const float roll = m_props.GetRotation().z;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		// Пересчитываем правый и верхний вектор
		const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, front));

		glm::mat4 view = glm::lookAt(m_props.GetPosition(), m_props.GetPosition() + front, up);

		pScene->GetRenderer()->SetViewTransform(view);
		return true;
	}

}