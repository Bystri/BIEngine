#pragma once

#include <memory>

#include "Shader.h"
#include "Material.h"
#include "Sprite.h"
#include "Mesh.h"

namespace BIEngine
{
    //Отвечает за отрисвоку спрайтов 
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Shader> pShader);
        Renderer(const Renderer& orig) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;

        void SetProjection(const glm::mat4& proj);
        void SetViewTransform(const glm::mat4& view);
        void SetModelTransform(glm::vec2 position, glm::vec2 size, float rotate);
        void SetColor(glm::vec3 color);

        void DrawSprite(std::shared_ptr<Sprite> sprite);
        void DrawMesh(std::shared_ptr<Mesh> mesh);

    private:
        //Скомпилированная шейдер программа, с помощью которой идет отрисовка спрайтов
        std::shared_ptr<Shader> m_pShader;        
    };
}