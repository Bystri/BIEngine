#pragma once

#include "../../BIEngine/StdLib/SharedPtr.h"
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
   ActorPickerInfoStorage(BIEngine::SharedPtr<BIEngine::Framebuffer> pFramebuffer)
      : m_pFramebuffer(pFramebuffer)
   {
   }

private:
   BIEngine::SharedPtr<BIEngine::Framebuffer> m_pFramebuffer;
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

   BIEngine::WeakPtr<ActorPickerInfoStorage> GetPickingInfoStorage() const;

   bool Init() override;

   void OnRender(BIEngine::Scene* const pScene, BIEngine::RenderItemsStorage* const pStorage);

private:
   const int m_screenWidth;
   const int m_screenHeight;

   BIEngine::SharedPtr<ActorPickerInfoStorage> m_pActorPickerInfoStorage;

   BIEngine::SharedPtr<BIEngine::Framebuffer> m_framebuffer;
   BIEngine::SharedPtr<BIEngine::Texture2D> m_pickingTexture;
   BIEngine::SharedPtr<BIEngine::Renderbuffer> m_depthBuffer;

   BIEngine::SharedPtr<BIEngine::ShaderProgram> m_pShaderProgram;
};
