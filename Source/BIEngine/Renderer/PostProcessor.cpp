#include "PostProcessor.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine {

PostProcessor::PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram)
   : m_screenOutputMesh(MeshGeometryGenerator::CreateGrid(2.0f, 2.0f, 2, 2)), m_pShaderProgram(pShaderProgram)
{
}

void PostProcessor::Use(Renderer* const pRenderer, std::shared_ptr<Texture2D> pTextureToProcess)
{
   RenderCommand renderCommand(m_screenOutputMesh.GetVao(), m_screenOutputMesh.GetIndices().size(), m_pShaderProgram);

   renderCommand.RenderState.Cull = false;
   renderCommand.RenderState.DepthTest = false;
   renderCommand.GetShaderProgramState().AddTexture(0, pTextureToProcess);

   pRenderer->DrawRenderCommand(renderCommand);
}

} // namespace BIEngine
