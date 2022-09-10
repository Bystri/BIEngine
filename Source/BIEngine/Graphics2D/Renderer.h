#pragma once

#include <memory>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace BIEngine
{
    //�������� �� ��������� �������� 
    class Renderer
    {
    public:
        Renderer(Shader& shader);
        Renderer(const Renderer& orig) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;

        ~Renderer();

        void SetViewTransform(const glm::mat4& view);

        void DrawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 position,
            glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));

    private:
        //���������������� ������ ���������, � ������� ������� ���� ��������� ��������
        Shader       m_shader;        
        unsigned int m_quadVAO;

        void initRenderData();
    };
}