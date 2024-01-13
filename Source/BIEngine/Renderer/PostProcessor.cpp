#include "PostProcessor.h"

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine {

PostProcessor::PostProcessor(std::shared_ptr<ShaderProgram> pShaderProgram, std::shared_ptr<Framebuffer> pFramebuffer)
   : m_screenOutputMesh(MeshGeometryGenerator::CreateGrid(2.0f, 2.0f, 2, 2)), m_pShaderProgram(pShaderProgram), m_pFramebuffer(pFramebuffer)
{
}

void PostProcessor::Use(std::shared_ptr<Texture2D> pTextureToProcess)
{
   m_pFramebuffer->Bind();
   m_pShaderProgram->Use();
   pTextureToProcess->Bind(0);
   glBindVertexArray(m_screenOutputMesh.GetVao());
   glDrawElements(GL_TRIANGLES, m_screenOutputMesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
}

} // namespace BIEngine
