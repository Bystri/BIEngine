#pragma once

#include <memory>

#include "../../BIEngine/Renderer/Renderbuffer.h"
#include "../../BIEngine/Renderer/Framebuffer.h"
#include "../../BIEngine/Renderer/Texture.h"
#include "../../BIEngine/Renderer/ShadersLoader.h"
#include "../../BIEngine/Graphics/GraphicsTechnique.h"

class ActorPickerInfoStorage {
   friend class ActorPickingTechnique;

public:
   BIEngine::ActorId GetActorId(const int x, const int y);

private:
   ActorPickerInfoStorage(std::shared_ptr<BIEngine::Framebuffer> pFramebuffer)
      : m_pFramebuffer(pFramebuffer)
   {
   }

private:
   std::shared_ptr<BIEngine::Framebuffer> m_pFramebuffer;
};

class ActorPickingTechnique : public BIEngine::IGraphicsTechnique {
public:
   ActorPickingTechnique(int screenWidth, int screenHeight)
      : m_screenWidth(screenWidth), m_screenHeight(screenHeight),
        m_pActorPickerInfoStorage(nullptr),
        m_framebuffer(nullptr), m_pickingTexture(nullptr), m_depthBuffer(nullptr),
        m_pShaderProgram(nullptr)
   {
   }

   std::weak_ptr<ActorPickerInfoStorage> GetPickingInfoStorage() const;

   bool Init() override;

   void OnRender(BIEngine::Scene* const pScene, BIEngine::RenderItemsStorage* const pStorage);

private:
   const int m_screenWidth;
   const int m_screenHeight;

   std::shared_ptr<ActorPickerInfoStorage> m_pActorPickerInfoStorage;

   std::shared_ptr<BIEngine::Framebuffer> m_framebuffer;
   std::shared_ptr<BIEngine::Texture2D> m_pickingTexture;
   std::shared_ptr<BIEngine::Renderbuffer> m_depthBuffer;

   std::shared_ptr<BIEngine::ShaderProgram> m_pShaderProgram;
};
