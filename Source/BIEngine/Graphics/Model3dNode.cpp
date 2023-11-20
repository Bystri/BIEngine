#include "Model3dNode.h"

namespace BIEngine
{

	bool Model3dNode::OnRender(Scene* pScene)
	{
		RenderCommand renderCommand(m_pModel->GetMesh(), m_props.GetMaterial()->GetShaderProgramPtr());

		renderCommand.RenderState = m_props.GetMaterial()->GetRenderState();

		renderCommand.GetShaderProgramState().SetVector3f("objectColor", m_props.GetMaterial()->GetColor());

		renderCommand.pTexture = m_pModel->GetTexture();

		pScene->GetRenderer()->DrawRenderCommand(renderCommand);
		return true;
	}

}