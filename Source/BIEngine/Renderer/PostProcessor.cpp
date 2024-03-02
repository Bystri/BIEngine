#include "PostProcessor.h"

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine {

PostProcessor::PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram)
   : m_screenOutputMesh(MeshGeometryGenerator::CreateGrid(2.0f, 2.0f, 2, 2)), m_pShaderProgram(pShaderProgram)
{
}

void PostProcessor::Use(std::shared_ptr<Texture2D> pTextureToProcess, std::shared_ptr<Framebuffer> pRenderTarget)
{
   pRenderTarget->Bind();
   m_pShaderProgram->Use();
   pTextureToProcess->Bind(0);
   glBindVertexArray(m_screenOutputMesh.GetVao());
   glDrawElements(GL_TRIANGLES, m_screenOutputMesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
}

} // namespace BIEngine
