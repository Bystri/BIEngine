#pragma once

#include "SelectedActorOutliner.h"

bool SelectedActorOutliner::Init()
{
   const std::string spEmptyColorPath = "effects/emptyColor.sp";
   auto emptyColorSp = std::static_pointer_cast<BIEngine::ShaderProgramData>(BIEngine::ResCache::Get()->GetHandle(spEmptyColorPath)->GetExtra());
   m_pEmptyColorSp = emptyColorSp->GetShaderProgram();

   const std::string spSolidColorPath = "effects/solidColor.sp";
   auto solidColorSp = std::static_pointer_cast<BIEngine::ShaderProgramData>(BIEngine::ResCache::Get()->GetHandle(spSolidColorPath)->GetExtra());
   m_pSolidColorSp = solidColorSp->GetShaderProgram();

   return true;
}

void selectedActorOutlinerScaleAxis(glm::vec4& axis, const float acc)
{
   const float axisLength = glm::length(axis);
   axis *= (axisLength + 0.2f) / axisLength;
}

void SelectedActorOutliner::OnRender(BIEngine::Scene* const pScene, BIEngine::RenderItemsStorage* const pStorage)
{
   auto& opaqueItems = pStorage->GetOpaqueRenderItems();

   pScene->GetRenderer()->GetRenderDevice().SetStencilOp(BIEngine::RenderDevice::StencilAction::KEEP, BIEngine::RenderDevice::StencilAction::KEEP, BIEngine::RenderDevice::StencilAction::REPLACE);

   for (const auto& ritem : opaqueItems) {
      if (ritem.actorId != m_pActorEditorWidget->GetCurrentSelectedActorId()) {
         continue;
      }

      BIEngine::RenderCommand renderCommandStencil(ritem.VAO, ritem.IndicesSize, m_pEmptyColorSp);
      renderCommandStencil.Transform = ritem.ModelTransform;
      renderCommandStencil.RenderState.DepthTest = false;
      renderCommandStencil.RenderState.StencilTest = true;
      renderCommandStencil.RenderState.StencilWrite = true;
      renderCommandStencil.RenderState.StencilAction = BIEngine::RenderDevice::Func::ALWAYS;
      renderCommandStencil.RenderState.StencilFuncRefValue = 1;
      renderCommandStencil.RenderState.StencilFuncMask = 0xFF;

      pScene->GetRenderer()->DrawRenderCommand(renderCommandStencil);

      BIEngine::RenderCommand renderCommandOutline(ritem.VAO, ritem.IndicesSize, m_pSolidColorSp);
      glm::mat4 scaledMatrix = ritem.ModelTransform;

      constexpr float outlineWidth = 0.2f;
      selectedActorOutlinerScaleAxis(scaledMatrix[0], outlineWidth);
      selectedActorOutlinerScaleAxis(scaledMatrix[1], outlineWidth);
      selectedActorOutlinerScaleAxis(scaledMatrix[2], outlineWidth);

      renderCommandOutline.Transform = scaledMatrix;

      renderCommandOutline.RenderState.DepthTest = false;
      renderCommandOutline.RenderState.StencilTest = true;
      renderCommandOutline.RenderState.StencilAction = BIEngine::RenderDevice::Func::NOTEQUAL;
      renderCommandOutline.RenderState.StencilFuncRefValue = 1;
      renderCommandOutline.RenderState.StencilFuncMask = 0xFF;

      renderCommandOutline.GetShaderProgramState().SetColorRgba("color", BIEngine::COLOR_RED);

      pScene->GetRenderer()->DrawRenderCommand(renderCommandOutline);
   }
}
