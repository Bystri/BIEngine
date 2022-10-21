#pragma once

#include "SceneNodes.h"
#include "Mesh.h"
#include "Scene.h"

namespace BIEngine
{
	//Графический узел, отвечающий за работу с мешами в OpenGL
	class MeshNode : public SceneNode
	{
	public:
		MeshNode(const ActorId actorId,
			RenderLayer renderLayer)
			: SceneNode(actorId, renderLayer)
			, m_pMesh(nullptr)
		{
		}

		virtual ~MeshNode() {}

		void SetMesh(std::shared_ptr<Mesh> mesh) { m_pMesh = mesh; }
		std::shared_ptr<Mesh> GetMesh() const { return m_pMesh; }

		virtual bool OnRender(Scene* pScene);

	protected:
		std::shared_ptr<Mesh> m_pMesh;
	};

}