#include "SpriteNode.h"

namespace BIEngine {

bool SpriteNode::OnRender(Scene* pScene)
{
   RenderCommand renderCommand(m_pSprite->GetMesh()->GetVao(), m_pSprite->GetMesh()->GetIndices().size(), m_pSprite->GetShaderProgramPtr());

   renderCommand.RenderState = m_pSprite->GetRanderState();
   renderCommand.RenderState.Cull = true;
   renderCommand.Transform = GetLocalModelMatrix();

   renderCommand.GetShaderProgramState() = m_pSprite->ConstructShaderProgramState();

   pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   return true;
}

} // namespace BIEngine
