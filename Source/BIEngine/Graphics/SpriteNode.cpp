#include "SpriteNode.h"

namespace BIEngine
{

	bool SpriteNode::OnRender(Scene* pScene)
	{
		RenderCommand renderCommand(m_pSprite->GetMesh(), m_props.GetMaterial()->GetShaderProgramPtr());

		renderCommand.RenderState = m_props.GetMaterial()->GetRenderState();
		
		renderCommand.GetShaderProgramState().SetVector3f("objectColor", m_props.GetMaterial()->GetColor());

		renderCommand.pTexture = m_pSprite->GetTexture();

		pScene->GetRenderer()->DrawRenderCommand(renderCommand);
		return true;
	}

}