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

    void Renderer::SetModelTransform(glm::vec2 position, glm::vec2 size, float rotate)
    {
        glm::mat4 model = glm::mat4(1.0f);

        //Мы сдвигаем спрайт так, чтобы опорная точка объекта была по центру, а не слева-сверху
        position.x -= size.x * 0.5f;
        position.y -= size.y * 0.5f;
        model = glm::translate(model, glm::vec3(position, 0.0f));

        //Чтобы повернуть спрайт вокруг центра, мы должны сначала его сдвинуть, затем повернуть, и вернуть спрайт на место
        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size, 1.0f));

        m_pShader->Use();
        m_pShader->SetMatrix4("model", model);
    }

    void Renderer::SetColor(glm::vec3 color)
    {
        m_pShader->Use();
        m_pShader->SetVector3f("objectColor", color);
    }

    void Renderer::DrawSprite(std::shared_ptr<Sprite> sprite)
    {
        if (!sprite->GetTexture())
            return;

        glActiveTexture(GL_TEXTURE0);
        sprite->GetTexture()->Bind();

        glBindVertexArray(sprite->GetMesh()->m_VAO);
        glDrawElements(GL_TRIANGLES, sprite->GetMesh()->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::DrawMesh(std::shared_ptr<Mesh> mesh)
    {
        glBindVertexArray(mesh->m_VAO);
        glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}