#include "ModelNode.h"

namespace BIEngine {

bool ModelNode::OnRender(Scene* pScene)
{
   const std::vector<std::shared_ptr<ModelMesh>>& modelMeshes = m_pModel->GetMeshes();

   for (const auto& pModelMesh : modelMeshes) {
      RenderCommand renderCommand(pModelMesh->GetMesh(), pModelMesh->GetMaterial()->GetShaderProgramPtr());

      renderCommand.RenderState = pModelMesh->GetMaterial()->GetRenderState();
      renderCommand.RenderState.Cull = !pModelMesh->GetMaterial()->IsDoubleSided();
      renderCommand.Transform = GetLocalModelMatrix();

      renderCommand.GetShaderProgramState() = pModelMesh->GetMaterial()->ConstructShaderProgramState();
      renderCommand.GetShaderProgramState().AddTexture(pScene->GetShadowMap());
      pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   }

   return true;
}

} // namespace BIEngine
