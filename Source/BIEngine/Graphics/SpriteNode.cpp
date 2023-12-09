#include "SpriteNode.h"

namespace BIEngine
{

	bool SpriteNode::OnRender(Scene* pScene)
	{
		RenderCommand renderCommand(m_pSprite->GetMesh(), m_props.GetMaterial()->GetShaderProgramPtr());

		renderCommand.RenderState = m_props.GetMaterial()->GetRenderState();
		renderCommand.Transform = GetLocalModelMatrix();

		renderCommand.GetShaderProgramState().SetVector3f("objectColor", m_props.GetMaterial()->GetColor());
		renderCommand.GetShaderProgramState().SetInteger("sprite", 0);

		renderCommand.pTextures.push_back(m_pSprite->GetTexture());

		pScene->GetRenderer()->DrawRenderCommand(renderCommand);
		return true;
	}

}