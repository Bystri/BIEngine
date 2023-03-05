#pragma once

#include <memory>

#include "Shader.h"
#include "Material.h"
#include "Sprite.h"
#include "Model3d.h"

namespace BIEngine
{
    //Отвечает за отрисвоку спрайтов 
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Shader> pShader);
        Renderer(const Renderer& orig) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;

        void Init();

        void SetProjection(const glm::mat4& proj);
        void SetViewTransform(const glm::mat4& view);
        void SetModelTransform(const glm::mat4& model);
        void SetColor(glm::vec3 color);

        void DrawSprite(std::shared_ptr<Sprite> pSprite);
        void DrawModel(std::shared_ptr<Model3d> pModel);
        void DrawLine(const glm::vec3& pointFrom, const glm::vec3& pointTo, const glm::vec3& color);

    private:
        //Скомпилированная шейдер программа, с помощью которой идет отрисовка спрайтов
        std::shared_ptr<Shader> m_pShader;    

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
    };
}