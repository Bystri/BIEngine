#include "SpriteNode.h"

namespace BIEngine
{

	bool SpriteNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->DrawSprite(m_props.GetMaterial()->GetRenderState(), m_props.GetMaterial()->GetShaderProgramState(), m_props.GetMaterial()->GetColor(), m_pSprite);
		return true;
	}

}