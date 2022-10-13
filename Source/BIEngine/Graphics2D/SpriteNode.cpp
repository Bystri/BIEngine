#include "SpriteNode.h"

namespace BIEngine
{

	bool SpriteNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->DrawSprite(m_pTexture, m_props.GetMaterial()->GetColor());
		return true;
	}

}