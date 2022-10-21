#include "MeshNode.h"

namespace BIEngine
{

	bool MeshNode::OnRender(Scene* pScene)
	{
		pScene->GetRenderer()->SetColor(m_props.GetMaterial()->GetColor());
		pScene->GetRenderer()->DrawMesh(m_pMesh);
		return true;
	}

}