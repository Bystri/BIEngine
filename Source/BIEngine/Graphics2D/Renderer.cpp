#include "Renderer.h"

namespace BIEngine
{

    Renderer::Renderer(std::shared_ptr<Shader> pShader)
    {
        m_pShader = pShader;
        initRenderData();
    }

    Renderer::~Renderer()
    {
        glDeleteVertexArrays(1, &m_quadVAO);
    }

    void Renderer::initRenderData()
    {
        unsigned int VBO;
        float vertices[] = {
            // Позиция     // Текстура
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(m_quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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

    void Renderer::DrawSprite(std::shared_ptr<Texture2D> pTexture, glm::vec3 color)
    {
        if (!pTexture)
            return;

        m_pShader->Use();
        m_pShader->SetVector3f("spriteColor", color);

        glActiveTexture(GL_TEXTURE0);
        pTexture->Bind();

        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

}