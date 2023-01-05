#include "Renderer.h"

namespace BIEngine
{

    Renderer::Renderer(std::shared_ptr<Shader> pShader)
    {
        m_pShader = pShader;
    }

    void Renderer::SetProjection(const glm::mat4& proj)
    {
        m_pShader->Use();
        m_pShader->SetMatrix4("projection", proj);
    }

    void Renderer::SetViewTransform(const glm::mat4& view)
    {
        m_pShader->Use();
        m_pShader->SetMatrix4("view", view);
    }

    void Renderer::SetModelTransform(const glm::mat4& model)
    {   
        m_pShader->Use();
        m_pShader->SetMatrix4("model", model);
    }

    void Renderer::SetColor(glm::vec3 color)
    {
        m_pShader->Use();
        m_pShader->SetVector3f("objectColor", color);
    }

    void Renderer::DrawSprite(std::shared_ptr<Sprite> pSprite)
    {
        if (!pSprite->GetTexture())
            return;

        m_pShader->Use();
        glActiveTexture(GL_TEXTURE0);
        pSprite->GetTexture()->Bind();

        glBindVertexArray(pSprite->GetMesh()->m_VAO);
        glDrawElements(GL_TRIANGLES, pSprite->GetMesh()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::DrawModel(std::shared_ptr<Model3d> pModel)
    {
        if (!pModel->GetTexture())
            return;

        glActiveTexture(GL_TEXTURE0);
        pModel->GetTexture()->Bind();

        glBindVertexArray(pModel->GetMesh()->m_VAO);
        glDrawElements(GL_TRIANGLES, pModel->GetMesh()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}