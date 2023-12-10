#include "Model3dNode.h"

namespace BIEngine {

bool Model3dNode::OnRender(Scene* pScene)
{
   RenderCommand renderCommand(m_pModel->GetMesh(), m_props.GetMaterial()->GetShaderProgramPtr());

   renderCommand.RenderState = m_props.GetMaterial()->GetRenderState();
   renderCommand.Transform = GetLocalModelMatrix();

   renderCommand.GetShaderProgramState().SetVector3f("material.color", m_props.GetMaterial()->GetColor());
   renderCommand.GetShaderProgramState().SetInteger("material.diffuse", 0);
   renderCommand.GetShaderProgramState().SetInteger("material.specular", 1);
   renderCommand.GetShaderProgramState().SetFloat("material.shininess", m_props.GetMaterial()->GetShininess());

   renderCommand.pTextures.push_back(m_props.GetMaterial()->GetDiffuseMap());
   renderCommand.pTextures.push_back(m_props.GetMaterial()->GetSpecularMap());

   pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   return true;
}

} // namespace BIEngine