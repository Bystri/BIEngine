#include "Model3dNode.h"

namespace BIEngine
{

	bool Model3dNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->DrawModel(m_props.GetMaterial()->GetRenderState(), m_props.GetMaterial()->GetShaderProgramState(), m_props.GetMaterial()->GetColor(), m_pModel);
		return true;
	}

}