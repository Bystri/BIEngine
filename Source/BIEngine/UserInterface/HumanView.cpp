#include "HumanView.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/ImageLoader.h"
#include "../EngineCore/GameApp.h"
#include "../Audio/irrKlangAudio.h"

namespace BIEngine {

const unsigned int SCREEN_REFRESH_RATE = 1000 / 60;

HumanView::HumanView(unsigned int screenWidth, unsigned int screenHeight)
   : m_viewId(INVALID_GAME_VIEW_ID), m_userInterface(), m_pRenderer(nullptr), m_pScene(nullptr),
     m_currTick(0.0f), m_lastDraw(0.0f), m_isRunFullSpeed(true), m_pointerRadius(1.0f), m_pKeyboardHandler(),
     m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
}

HumanView::~HumanView()
{
}

bool HumanView::Init()
{
   if (!g_pAudio) {
      g_pAudio = new irrKlangAudio();
   }

   if (!g_pAudio) {
      return false;
   }

   if (!g_pAudio->Initialize()) {
      return false;
   }

   if (!m_userInterface.Init(m_screenWidth, m_screenHeight)) {
      return false;
   }

   // Создание отображения
   m_pRenderer = std::make_shared<Renderer>();
   if (!m_pRenderer->Init(m_screenWidth, m_screenHeight)) {
      return false;
   }
   // Создания сцены на основе отображения
   m_pScene = std::make_unique<Scene>(m_pRenderer);
   m_pScene->Init();

   m_pScene->SetCamera(std::make_shared<Camera>());

   return true;
}

void HumanView::Shutdown()
{
   m_userInterface.Shutdown();

   if (g_pAudio) {
      g_pAudio->Shutdown();
      delete g_pAudio;
      g_pAudio = nullptr;
   }
}

void HumanView::OnUpdate(const GameTimer& gt)
{
   m_pScene->OnUpdate(gt);
}

void HumanView::OnRender(const GameTimer& gt)
{
   m_pScene->OnRender(gt);
   m_userInterface.OnRender(gt);
}

void HumanView::OnPointerMove(float xpos, float ypos)
{
   if (m_pPointerHandler) {
      m_pPointerHandler->OnPointerMove(IPointerHandler::Point{xpos, ypos}, m_pointerRadius);
   }
}

void HumanView::SetPointerButton(int button, bool state)
{
   if (m_pPointerHandler) {
      if (state) {
         m_pPointerHandler->OnPointerButtonDown(IPointerHandler::Point(), 0.0f, button);
      } else {
         m_pPointerHandler->OnPointerButtonUp(IPointerHandler::Point(), 0.0f, button);
      }
   }
}

void HumanView::SetKey(int key, int scancode, bool state)
{
   if (m_pKeyboardHandler) {
      if (state) {
         m_pKeyboardHandler->OnKeyDown(key, scancode);
      } else {
         m_pKeyboardHandler->OnKeyUp(key, scancode);
      }
   }
}

} // namespace BIEngine
