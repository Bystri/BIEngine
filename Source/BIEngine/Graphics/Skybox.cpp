#include "Skybox.h"

#include "Scene.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine {

Skybox::Skybox(std::shared_ptr<CubemapTexture> pCubemapTexture, std::shared_ptr<ShaderProgram> pShaderProgram)
   : m_cubeMesh(MeshGeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 0)), m_pCubemapTexture(pCubemapTexture), m_pShaderProgram(pShaderProgram)
{
}

bool Skybox::OnRender(Scene* pScene)
{
   RenderCommand renderCommand(std::make_shared<Mesh>(m_cubeMesh), m_pShaderProgram);

   renderCommand.RenderState.DepthWrite = false;
   renderCommand.RenderState.DepthCompare = RenderDevice::Func::LEQUAL;
   renderCommand.RenderState.Cull = false;

   glm::mat4 skyboxView = glm::mat4(glm::mat3(pScene->GetCamera()->GetViewMatrix()));

   renderCommand.GetShaderProgramState().SetMatrix4("skyboxView", skyboxView);
   renderCommand.GetShaderProgramState().SetInteger("samplerCube", 0);

   renderCommand.pTextures.push_back(m_pCubemapTexture);

   pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   return true;
}

} // namespace BIEngine