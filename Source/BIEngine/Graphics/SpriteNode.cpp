#include "SpriteNode.h"

namespace BIEngine
{

	bool SpriteNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->SetColor(m_props.GetMaterial()->GetColor());
		pScene->GetRenderer()->DrawSprite(m_pSprite);
		return true;
	}

}