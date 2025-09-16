#include "OpaqueGraphicsTechnique.h"

#include "../StdLib/Algorithm.h"

namespace BIEngine {

void OpaqueGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   const auto& dirLights = pStorage->GetDirectionalLightItems();
   const auto& pointLights = pStorage->GetPointLightItems();

   Sort(opaqueItems.Begin(), opaqueItems.End(), [](const RenderItemsStorage::OpaqueRenderItem& l, const RenderItemsStorage::OpaqueRenderItem& r) { return l.pMaterial->GetShaderProgramPtr()->GetId() < r.pMaterial->GetShaderProgramPtr()->GetId(); });

   int lastShaderId = -1;

   for (const auto& ritem : opaqueItems) {

      auto currentShader = ritem.pMaterial->GetShaderProgramPtr();

      if (lastShaderId != currentShader->GetId()) {

         currentShader->Use();

         for (int i = 0; i < dirLights.Size(); ++i) {
            currentShader->SetMatrix4("dirLightShadowInfos[" + ToString(i) + "].dirLightSpaceMatrix", dirLights[i].LightMatr);
            currentShader->SetInteger("dirLightShadowInfos[" + ToString(i) + "].shadowMap", 10 + i);
            dirLights[i].pShadowMap->Bind(10 + i);
         }

         for (int i = 0; i < pointLights.Size(); ++i) {
            currentShader->SetVector3f("pointLightShadowInfos[" + ToString(i) + "].lightPos", pointLights[i].position);
            currentShader->SetInteger("pointLightShadowInfos[" + ToString(i) + "].shadowMap", 10 + i + dirLights.Size());
            pointLights[i].pShadowMap->Bind(10 + i + dirLights.Size());
         }

         lastShaderId = currentShader->GetId();
      }

      RenderCommand renderCommand(ritem.VAO, ritem.IndicesSize, currentShader);

      renderCommand.RenderState = ritem.pMaterial->GetRenderState();
      renderCommand.RenderState.Cull = !ritem.pMaterial->IsDoubleSided();
      renderCommand.Transform = ritem.ModelTransform;

      renderCommand.GetShaderProgramState() = ritem.pMaterial->ConstructShaderProgramState();

      pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   }
}

} // namespace BIEngine
