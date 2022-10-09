#pragma once

#include <memory>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace BIEngine
{
    //Отвечает за отрисвоку спрайтов 
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Shader> pShader);
        Renderer(const Renderer& orig) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;

        ~Renderer();

        void SetViewTransform(const glm::mat4& view);

        void DrawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 position,
            glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));

    private:
        //Скомпилированная шейдер программа, с помощью которой идет отрисовка спрайтов
        std::shared_ptr<Shader> m_pShader;        
        unsigned int m_quadVAO;

        void initRenderData();
    };
}