#pragma once

#include "ActorPickingTechnique.h"

BIEngine::ActorId ActorPickerInfoStorage::GetActorId(const int x, const int y)
{
   m_pFramebuffer->Bind();
   glReadBuffer(GL_COLOR_ATTACHMENT0);

   float info[3];
   glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, info);

   glReadBuffer(GL_NONE);

   if (info[0] < 1.0f) {
      return BIEngine::Actor::INVALID_ACTOR_ID;
   }

   return static_cast<BIEngine::ActorId>(info[0] - 1.0f);
}

std::weak_ptr<ActorPickerInfoStorage> ActorPickingTechnique::GetPickingInfoStorage() const
{
   return m_pActorPickerInfoStorage;
}

bool ActorPickingTechnique::Init()
{
   const std::string spPath = "effects/actorPicking.sp";
   auto pickingShaderProgram = std::static_pointer_cast<BIEngine::ShaderProgramData>(BIEngine::ResCache::Get()->GetHandle(spPath)->GetExtra());
   m_pShaderProgram = pickingShaderProgram->GetShaderProgram();

   m_framebuffer = std::make_shared<BIEngine::Framebuffer>();

   BIEngine::Texture2D::CreationParams params;
   params.DataType = BIEngine::Texture2D::Type::FLOAT;
   m_pickingTexture = BIEngine::Texture2D::Create(m_screenWidth, m_screenHeight, BIEngine::Texture::SizedFormat::R_32_F, BIEngine::Texture::Format::RED, nullptr, params);

   m_depthBuffer = BIEngine::Renderbuffer::Create(m_screenWidth, m_screenHeight, BIEngine::Renderbuffer::Format::DEPTH24);

   FramebufferAttach(m_framebuffer, BIEngine::FramebufferAttachementType::COLOR, m_pickingTexture);
   FramebufferAttach(m_framebuffer, BIEngine::FramebufferAttachementType::DEPTH, m_depthBuffer);

   bool ret = m_framebuffer->Check();
   assert(ret, "Piacking framebuffer is corrupted");
   if (!ret) {
      return false;
   }

   m_pActorPickerInfoStorage = std::shared_ptr<ActorPickerInfoStorage>(new ActorPickerInfoStorage(m_framebuffer));

   return ret;
}

void ActorPickingTechnique::OnRender(BIEngine::Scene* const pScene, BIEngine::RenderItemsStorage* const pStorage)
{
   auto& opaqueItems = pStorage->GetOpaqueRenderItems();

   m_framebuffer->Bind();
   glViewport(0, 0, pScene->GetRenderer()->GetScreenWidth(), pScene->GetRenderer()->GetScreenHeight());
   static const BIEngine::ColorRgba CLEAR_COLOR = BIEngine::ColorRgba(0.0f, 0.0f, 0.0f, 1.0f);
   pScene->GetRenderer()->Clear(BIEngine::RenderDevice::ClearFlag::COLOR | BIEngine::RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);

   for (const auto& ritem : opaqueItems) {
      BIEngine::RenderCommand renderCommand(ritem.pMesh.get(), m_pShaderProgram);
      renderCommand.Transform = ritem.ModelTransform;
      renderCommand.GetShaderProgramState().SetInteger("actorIndex", ritem.actorId);

      pScene->GetRenderer()->DrawRenderCommand(renderCommand);
   }
}
