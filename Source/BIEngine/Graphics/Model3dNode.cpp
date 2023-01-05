#include "Model3dNode.h"

namespace BIEngine
{

	bool Model3dNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->SetColor(m_props.GetMaterial()->GetColor());
		pScene->GetRenderer()->DrawModel(m_pModel);
		return true;
	}

}