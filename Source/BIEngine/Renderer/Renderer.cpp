#include "Renderer.h"

#include "../Utilities/DebugDraw.h"

namespace BIEngine
{

    Renderer::Renderer()
    {
    }

    void Renderer::Init()
    {
    }

    void Renderer::SetProjection(const glm::mat4& proj)
    {
        m_projMatrix = proj;
        DebugDraw::SetProjection(proj);
    }

    void Renderer::SetViewTransform(const glm::mat4& view)
    {
        m_viewMatrix = view;
        DebugDraw::SetViewTransform(view);
    }

    void Renderer::SetModelTransform(const glm::mat4& model)
    {   
        m_modelMatrix = model;
    }

    void Renderer::DrawSprite(RenderState renderState, ShaderProgramState& pShaderProgramState, Color color, std::shared_ptr<Sprite> pSprite)
    {
        if (!pSprite->GetTexture())
            return;

        m_renderDevice.SetDepthTest(renderState.DepthTest);
        m_renderDevice.SetDepthFunc(renderState.DepthCompare);
        m_renderDevice.SetBlend(renderState.Blend);
        m_renderDevice.SetBlendFunc(renderState.BlendSrc, renderState.BlendDst);
        m_renderDevice.SetBlendEquation(renderState.BlendEquation);
        m_renderDevice.SetCull(renderState.Cull);
        m_renderDevice.SetCullFace(renderState.CullFace);
        m_renderDevice.SetCullWindingOrder(renderState.CullWindingOrder);

        pShaderProgramState.SetMatrix4("projection", m_projMatrix);
        pShaderProgramState.SetMatrix4("view", m_viewMatrix);
        pShaderProgramState.SetMatrix4("model", m_modelMatrix);
        pShaderProgramState.SetVector3f("objectColor", color);
        pShaderProgramState.Use();

        glActiveTexture(GL_TEXTURE0);
        pSprite->GetTexture()->Bind();

        glBindVertexArray(pSprite->GetMesh()->m_VAO);
        glDrawElements(GL_TRIANGLES, pSprite->GetMesh()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::DrawModel(RenderState renderState, ShaderProgramState& pShaderProgramState, Color color, std::shared_ptr<Model3d> pModel)
    {
        if (!pModel->GetTexture())
            return;

        m_renderDevice.SetDepthTest(renderState.DepthTest);
        m_renderDevice.SetDepthFunc(renderState.DepthCompare);
        m_renderDevice.SetBlend(renderState.Blend);
        m_renderDevice.SetBlendFunc(renderState.BlendSrc, renderState.BlendDst);
        m_renderDevice.SetBlendEquation(renderState.BlendEquation);
        m_renderDevice.SetCull(renderState.Cull);
        m_renderDevice.SetCullFace(renderState.CullFace);
        m_renderDevice.SetCullWindingOrder(renderState.CullWindingOrder);

        pShaderProgramState.SetMatrix4("projection", m_projMatrix);
        pShaderProgramState.SetMatrix4("view", m_viewMatrix);
        pShaderProgramState.SetMatrix4("model", m_modelMatrix);
        pShaderProgramState.SetVector3f("objectColor", color);
        pShaderProgramState.Use();

        glActiveTexture(GL_TEXTURE0);
        pModel->GetTexture()->Bind();

        glBindVertexArray(pModel->GetMesh()->m_VAO);
        glDrawElements(GL_TRIANGLES, pModel->GetMesh()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}