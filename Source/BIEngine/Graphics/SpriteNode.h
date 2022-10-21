#pragma once

#include "SceneNodes.h"
#include "Sprite.h"
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
			, m_pSprite(nullptr)
		{
		}

		virtual ~SpriteNode() {}

		void SetSprite(std::shared_ptr<Sprite> sprite) { m_pSprite = sprite; }
		std::shared_ptr<Sprite> GetSprite() const { return m_pSprite; }

		virtual bool OnRender(Scene* pScene);

	protected:
		std::shared_ptr<Sprite> m_pSprite;
	};

}