#pragma once

#include "SceneNodes.h"
#include "Texture.h"
#include "Scene.h"

namespace BIEngine
{
	//Графический узел, отвечающий за работу со спрайтами в OpenGL
	class SpriteNode : public SceneNode
	{
	public:
		SpriteNode(const ActorId actorId,
			RenderLayer renderLayer)
			: SceneNode(actorId, renderLayer)
			, m_pTexture(nullptr)
		{
		}

		virtual ~SpriteNode() {}

		void SetTexture(std::shared_ptr<Texture2D> texture) { m_pTexture = texture; }
		std::shared_ptr<Texture2D> GetTexture() const { return m_pTexture; }

		virtual bool OnRender(Scene* pScene);

	protected:
		std::shared_ptr<Texture2D> m_pTexture;
	};

}