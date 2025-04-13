#include "GameApp.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "../ResourceCache/ResCache.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/ImageLoader.h"
#include "../Renderer/TextureLoader.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/SkinnedMeshLoader.h"
#include "../Graphics/ModelLoader.h"
#include "../Graphics/AnimationLoader.h"
#include "../Graphics/MaterialLoader.h"
#include "../ResourceCache/XmlLoader.h"
#include "../UserInterface/HumanView.h"
#include "../Scripting/PythonStateManager.h"
#include "../Scripting/ScriptExports.h"
#include "../Scripting/ScriptResource.h"
#include "../Actors/ScriptComponent.h"

namespace BIEngine {

GameApp* g_pApp = nullptr;

// Константы для хранилища инициализации кэша ресурсов
const unsigned int RESOURCE_CACHE_SIZE_MB = 2 * 1024;    // Размер кэша ресурсов в Мб
const char* RESOURCE_CACHE_DIR_FILE_NAME = "../Assets";  // Имя папки-хранилища ресурсов для кэша. Мы делаем шаг назад, так как предпологается, что сырая папка ресурсов и папка редактора находтся в корне
const char* RESOURCE_CACHE_ZIP_FILE_NAME = "Assets.zip"; // Имя архива-хранилища ресурсов для кэша

const char* SCRIPT_PREINIT_FILE = "scripts/init.py";

GameApp::GameApp(std::shared_ptr<GameLogic> pGameLogic)
   : m_pGameLogic(pGameLogic)
{
   g_pApp = this;
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
   // Инициализация кэша ресурсов
   std::shared_ptr<IResourceFile> pZipFile;
   if (m_options.useDevelopmentAssets)
      pZipFile = std::make_shared<ResourceDirFile>(RESOURCE_CACHE_DIR_FILE_NAME);
   else
      pZipFile = std::make_shared<ResourceZipFile>(RESOURCE_CACHE_ZIP_FILE_NAME);

   ResCache::Create(RESOURCE_CACHE_SIZE_MB, pZipFile);

   ResCache::Get()->RegisterLoader(std::make_shared<XmlResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<JpgResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<PngResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<VertexShaderResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<FragmentShaderResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<GeometryShaderResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<UtilityShaderResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<ShaderProgramResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<ModelResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<SkinnedMeshResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<MeshResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<AnimationResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<ScriptResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<TextureResourceLoader>());
   ResCache::Get()->RegisterLoader(std::make_shared<MaterialResourceLoader>());
   // ResCache::Get()->RegisterLoader(CreateWavResourceLoader());

   // Создаем экземпляр одиночки нашей системы скриптов
   if (!PythonStateManager::Create()) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize Python interpreter");
      return false;
   }

   // Загружаем скрипт инициализации Python системы. Скрипт будет выполнен во время самой загрузки ресурса, поэтомы мы игнорируем возвращаемый хэндлер.
   ResCache::Get()->GetHandle(SCRIPT_PREINIT_FILE);

   m_pGameLogic->Init();

   return true;
}

std::shared_ptr<HumanView> GameApp::TryGetHumanView(unsigned int playerId)
{
   unsigned int currentPlayerId = 0;
   for (GameViewList::iterator i = m_pGameLogic->m_gameViews.begin(); i != m_pGameLogic->m_gameViews.end(); ++i) {
      if ((*i)->GetType() == GameViewType::Human) {
         if (currentPlayerId != playerId) {
            ++currentPlayerId;
            continue;
         }

         return std::static_pointer_cast<HumanView>(*i);
      }
   }

   return std::shared_ptr<HumanView>();
}

void GameApp::Close()
{
   m_pGameLogic->Terminate();

   // Освобождение кэша ресурсов
   ResCache::Destroy();
}

void GameApp::OnUpdate(GameTimer& gt)
{
   m_pGameLogic->OnUpdate(gt);
}

void GameApp::ProcessInput(const GameTimer& gt)
{
}

void GameApp::OnRender(const GameTimer& gt)
{
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   m_pGameLogic->OnRender(gt);

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameApp::OnPointerMove(float xpos, float ypos)
{
   m_pGameLogic->OnPointerMove(xpos, ypos);
}

void GameApp::SetPointerButton(int button, bool state)
{
   m_pGameLogic->SetPointerButton(button, state);
}

void GameApp::SetKey(int key, int scancode, bool pressed)
{
   if (key >= 0 && key < 1024) {
      m_pGameLogic->SetKey(key, scancode, pressed);
   }
}

} // namespace BIEngine
