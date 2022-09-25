#include "GameApp.h"

#include "../ResourceCache/ResCache.h"
#include "../Graphics2D/Opengl/ShadersLoaderOpenGL.h"
#include "../Graphics2D/Opengl/TextureLoaderOpenGL.h"
#include "../ResourceCache/XmlLoader.h"
#include "../UserInterface/HumanView.h"
#include "../Scripting/ScriptExports.h"
#include "../Scripting/ScriptProcess.h"
#include "../Scripting/ScriptResource.h"
#include "../Actors/ScriptComponent.h"

namespace BIEngine
{
	GameApp* g_pApp = nullptr;

	//Константы для хранилища инициализации кэша ресурсов
	const unsigned int RESOURCE_CACHE_SIZE = 50 * 1024; //Размер кэша ресурсов в Мб
	const char* RESOURCE_CACHE_DIR_FILE_NAME = "../Assets"; //Имя папки-хранилища ресурсов для кэша. Мы делаем шаг назад, так как предпологается, что сырая папка ресурсов и папка редактора находтся в корне
	const char* RESOURCE_CACHE_ZIP_FILE_NAME = "Assets.zip"; //Имя архива-хранилища ресурсов для кэша

	const char* SCRIPT_PREINIT_FILE = "scripts/init.lua";

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
		//Инициализация кэша ресурсов
		std::shared_ptr<IResourceFile> pZipFile;
		if (m_options.useDevelopmentAssets)
			pZipFile = std::make_shared<ResourceDirFile>(RESOURCE_CACHE_DIR_FILE_NAME);
		else
			pZipFile = std::make_shared<ResourceZipFile>(RESOURCE_CACHE_ZIP_FILE_NAME);

		ResCache::Create(RESOURCE_CACHE_SIZE, pZipFile);

		ResCache::Get()->RegisterLoader(std::make_shared<XmlResourceLoader>());
		ResCache::Get()->RegisterLoader(std::make_shared<PngResourceLoader>());
		ResCache::Get()->RegisterLoader(std::make_shared<VertexShaderResourceLoader>());
		ResCache::Get()->RegisterLoader(std::make_shared<FragmentShaderResourceLoader>());
		ResCache::Get()->RegisterLoader(std::make_shared<ScriptResourceLoader>());
		//ResCache::Get()->RegisterLoader(CreateScriptResourceLoader());
		//ResCache::Get()->RegisterLoader(CreateWavResourceLoader());

		//Создаем экземпляр одиночки нашей системы скриптов
		if (!LuaStateManager::Create())
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize Lua");
			return false;
		}

		//Загружаем скрипт инициализации lua системы. Скрипт будет выполнен во время самой загрузки ресурса, поэтомы мы игнорируем возвращаемый хэндлер. 
		ResCache::Get()->GetHandle(SCRIPT_PREINIT_FILE);

		//Регистрируем функции в скрипте, которые будут иметь доступ к системам движка, написанным на C++
		ScriptExports::Register();
		ScriptProcess::RegisterScriptClass();
		ScriptComponent::RegisterScriptFunctions();

		m_pGameLogic->Init();

		return true;
	}

	void GameApp::Close()
	{
		//Освобождение кэша ресурсов
		ResCache::Destroy();
	}

	void GameApp::OnUpdate(float dt)
	{
		m_pGameLogic->OnUpdate(dt);
	}

	void GameApp::ProcessInput(float dt)
	{

	}

	void GameApp::OnRender(float time, float dt)
	{
		m_pGameLogic->OnRender(time, dt);
	}

	void GameApp::InputProc(int key, int scancode, bool pressed)
	{
		if (key >= 0 && key < 1024)
		{
			m_pGameLogic->SetKey(key, scancode, pressed);
		}
	}

}