#include "HumanView.h"

#include "../Renderer/ShadersLoader.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/ImageLoader.h"
#include "../Graphics/SpriteNode.h"
#include "../EngineCore/GameApp.h"
#include "../Audio/irrKlangAudio.h"
#include "../Utilities/DebugDraw.h"

namespace BIEngine {

const unsigned int SCREEN_REFRESH_RATE = 1000 / 60;

HumanView::HumanView(unsigned int screenWidth, unsigned int screenHeight)
   : m_viewId(INVALID_GAME_VIEW_ID)

     ,
     m_userInterface(), m_pRenderer(nullptr), m_pScene(nullptr)

     ,
     m_currTick(0.0f), m_lastDraw(0.0f), m_isRunFullSpeed(true)

     ,
     m_pointerRadius(1.0f), m_pKeyboardHandler()

     ,
     m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
}

HumanView::~HumanView()
{
}

static std::shared_ptr<Skybox> humanViewCreateSkybox()
{
   auto xmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/scene.xml")->GetExtra());

   if (!xmlExtraData) {
      return nullptr;
   }

   tinyxml2::XMLElement* pSkyboxSettingsNode = xmlExtraData->GetRootElement()->FirstChildElement("Skybox");
   assert(pSkyboxSettingsNode);

   if (!pSkyboxSettingsNode) {
      return nullptr;
   }

   const char* vertexShaderPath;
   const char* fragmentShaderPath;
   pSkyboxSettingsNode->QueryStringAttribute("vertexShaderPath", &vertexShaderPath);
   pSkyboxSettingsNode->QueryStringAttribute("fragmentShaderPath", &fragmentShaderPath);

   if (strlen(vertexShaderPath) == 0 || strlen(fragmentShaderPath) == 0) {
      return nullptr;
   }

   std::shared_ptr<ShaderData> pVertShaderData = std::static_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(vertexShaderPath)->GetExtra());
   std::shared_ptr<ShaderData> pFragShaderxData = std::static_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(fragmentShaderPath)->GetExtra());
   std::shared_ptr<ShaderProgram> pShaderProgram = std::make_shared<ShaderProgram>();
   pShaderProgram->Compile(pVertShaderData->GetShaderIndex(), pFragShaderxData->GetShaderIndex());


   std::vector<std::string> faces{
      "cubemapTextureRightPath",
      "cubemapTextureLeftPath",
      "cubemapTextureTopPath",
      "cubemapTextureBottomPath",
      "cubemapTextureFrontPath",
      "cubemapTextureBackPath"};

   std::array<unsigned char*, 6> cubemapTextureImages;
   int width = -1;
   int height = -1;

   for (int i = 0; i < faces.size(); ++i) {
      const char* cubemapTexturePath;
      pSkyboxSettingsNode->QueryStringAttribute(faces[i].c_str(), &cubemapTexturePath);

      if (strlen(cubemapTexturePath) == 0) {
         return nullptr;
      }

      auto cubemapTextureResExtraData = std::static_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(cubemapTexturePath)->GetExtra());

      if (!cubemapTextureResExtraData) {
         return nullptr;
      }

      cubemapTextureImages[i] = cubemapTextureResExtraData->GetData();
      width = cubemapTextureResExtraData->GetWidth();
      height = cubemapTextureResExtraData->GetHeight();
   }

   std::shared_ptr<CubemapTexture> pTexture = std::make_shared<CubemapTexture>();
   pTexture->SetInternalFormat(GL_RGBA);
   pTexture->SetImageFormat(GL_RGBA);
   pTexture->Generate(width, height, cubemapTextureImages);

   return std::make_shared<Skybox>(pTexture, pShaderProgram);
}

bool HumanView::Init()
{
   if (!g_pAudio)
      g_pAudio = new irrKlangAudio();

   if (!g_pAudio)
      return false;

   if (!g_pAudio->Initialize())
      return false;

   m_userInterface.Init(m_screenWidth, m_screenHeight);

   // Создание отображения
   m_pRenderer = std::make_shared<Renderer>();
   m_pRenderer->Init();
   DebugDraw::Init();
   // Создания сцены на основе отображения
   m_pScene = new Scene(m_pRenderer);
   m_pScene->Init();

   m_pScene->SetCamera(std::make_shared<Camera>());
   m_pScene->SetSkybox(humanViewCreateSkybox());

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

   if (m_pScene) {
      delete m_pScene;
      m_pScene = nullptr;
   }
}

void HumanView::OnUpdate(const GameTimer& gt)
{
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
