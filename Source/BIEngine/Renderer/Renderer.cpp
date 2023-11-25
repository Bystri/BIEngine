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

        glGenBuffers(1, &m_uniformBufferBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferBlock);
        glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); // allocate 128 bytes of memory
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBufferBlock);
    }

    void Renderer::SetProjection(const glm::mat4& proj)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(proj));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        DebugDraw::SetProjection(proj);
    }

    void Renderer::SetViewTransform(const glm::mat4& view)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        DebugDraw::SetViewTransform(view);
    }

    void Renderer::SetModelTransform(const glm::mat4& model)
    {   
        m_modelMatrix = model;
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

        renderCommand.GetShaderProgramState().SetMatrix4("model", m_modelMatrix);
        renderCommand.GetShaderProgramState().Use();

        glActiveTexture(GL_TEXTURE0);
        renderCommand.pTexture->Bind();

        glBindVertexArray(renderCommand.GetMeshPtr()->m_VAO);
        glDrawElements(GL_TRIANGLES, renderCommand.GetMeshPtr()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

}