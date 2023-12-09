#include "Renderer.h"

#include "../Utilities/DebugDraw.h"

namespace BIEngine
{

    Renderer::Renderer()
    {
    }


    void Renderer::Init()
    {
        m_renderDevice.Init();
    }


    void Renderer::Clear(RenderDevice::ClearFlag flags, const Color& color)
    {
        m_renderDevice.Clear(flags, color);
    }


    void Renderer::DrawRenderCommand(RenderCommand& renderCommand)
    {
        const RenderState& renderState = renderCommand.RenderState;
        m_renderDevice.SetDepthTest(renderState.DepthTest);
        m_renderDevice.SetDepthWrite(renderState.DepthWrite);
        m_renderDevice.SetDepthFunc(renderState.DepthCompare);
        m_renderDevice.SetBlend(renderState.Blend);
        m_renderDevice.SetBlendFunc(renderState.BlendSrc, renderState.BlendDst);
        m_renderDevice.SetBlendEquation(renderState.BlendEquation);
        m_renderDevice.SetCull(renderState.Cull);
        m_renderDevice.SetCullFace(renderState.CullFace);
        m_renderDevice.SetCullWindingOrder(renderState.CullWindingOrder);

        renderCommand.GetShaderProgramState().SetMatrix4("model", renderCommand.Transform);
        renderCommand.GetShaderProgramState().Use();

        for (int i = 0; i < renderCommand.pTextures.size(); ++i) {
            renderCommand.pTextures[i]->Bind(i);
        }

        glBindVertexArray(renderCommand.GetMeshPtr()->m_VAO);
        glDrawElements(GL_TRIANGLES, renderCommand.GetMeshPtr()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

}