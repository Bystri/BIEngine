#include "OpaqueGraphicsTechnique.h"

namespace BIEngine {

void OpaqueGraphicsTechnique::OnRender(Scene* const pScene, RenderItemsStorage* const pStorage)
{
   auto& opaqueItems = pStorage->GetOpaqueRenderItems();
   const auto& dirLights = pStorage->GetDirectionalLightItems();
   const auto& pointLights = pStorage->GetPointLightItems();

   std::sort(opaqueItems.begin(), opaqueItems.end(), [](const RenderItemsStorage::OpaqueRenderItem& l, const RenderItemsStorage::OpaqueRenderItem& r) { return l.pMaterial->GetShaderProgramPtr()->GetId() < r.pMaterial->GetShaderProgramPtr()->GetId(); });

   int lastShaderId = -1;

   for (const auto& ritem : opaqueItems) {

      auto currentShader = ritem.pMaterial->GetShaderProgramPtr();

      if (lastShaderId != currentShader->GetId()) {

         currentShader->Use();

         for (int i = 0; i < dirLights.size(); ++i) {
            currentShader->SetMatrix4("dirLightShadowInfos[" + std::to_string(i) + "].dirLightSpaceMatrix", dirLights[i].LightMatr);
            currentShader->SetInteger("dirLightShadowInfos[" + std::to_string(i) + "].shadowMap", 10 + i);
            dirLights[i].pShadowMap->Bind(10 + i);
         }

         for (int i = 0; i < pointLights.size(); ++i) {
            currentShader->SetVector3f("pointLightShadowInfos[" + std::to_string(i) + "].lightPos", pointLights[i].position);
            currentShader->SetInteger("pointLightShadowInfos[" + std::to_string(i) + "].shadowMap", 10 + i + dirLights.size());
            pointLights[i].pShadowMap->Bind(10 + i + dirLights.size());
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
