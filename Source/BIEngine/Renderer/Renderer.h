#pragma once

#include <memory>

#include "ShaderProgram.h"
#include "../Graphics/Material.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/Model3d.h"

namespace BIEngine
{
    //Отвечает за отрисвоку спрайтов 
    class Renderer
    {
    public:
        Renderer();
        Renderer(const Renderer& orig) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;

        void Init();

        void SetProjection(const glm::mat4& proj);
        void SetViewTransform(const glm::mat4& view);
        void SetModelTransform(const glm::mat4& model);

        RenderDevice& GetRenderDevice() { return m_renderDevice; }

        void DrawSprite(RenderState renderState, ShaderProgramState& pShaderProgramState, Color color, std::shared_ptr<Sprite> pSprite);
        void DrawModel(RenderState renderState, ShaderProgramState& pShaderProgramState, Color color, std::shared_ptr<Model3d> pModel);

    private:
        RenderDevice m_renderDevice;

        glm::mat4 m_projMatrix;
        glm::mat4 m_viewMatrix;
        glm::mat4 m_modelMatrix;
    };
}