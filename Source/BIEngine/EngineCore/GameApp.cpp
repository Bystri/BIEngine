#include "GameApp.h"

#include "../ResourceCache/ResCache.h"
#include "../Graphics2D/Opengl/ShadersLoaderOpenGL.h"
#include "../Graphics2D/Opengl/TextureLoaderOpenGL.h"
#include "../ResourceCache/XmlLoader.h"
#include "../UserInterface/HumanView.h"
#include "../Scripting/ScriptExports.h"
#include "../Scripting/ScriptProcess.h"
#include "../Scripting/ScriptResource.h"

namespace BIEngine
{
	GameApp* g_pApp = nullptr;

	//��������� ��� ��������� ������������� ���� ��������
	const unsigned int RESOURCE_CACHE_SIZE = 50 * 1024; //������ ���� �������� � ��
	const char* RESOURCE_CACHE_DIR_FILE_NAME = "../Assets"; //��� �����-��������� �������� ��� ����. �� ������ ��� �����, ��� ��� ��������������, ��� ����� ����� �������� � ����� ��������� �������� � �����
	const char* RESOURCE_CACHE_ZIP_FILE_NAME = "Assets.zip"; //��� ������-��������� �������� ��� ����

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
		//������������� ���� ��������
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

		//������� ��������� �������� ����� ������� ��������
		if (!LuaStateManager::Create())
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize Lua");
			return false;
		}

		//��������� ������ ������������� lua �������. ������ ����� �������� �� ����� ����� �������� �������, ������� �� ���������� ������������ �������. 
		ResCache::Get()->GetHandle(SCRIPT_PREINIT_FILE);

		//������������ ������� � �������, ������� ����� ����� ������ � �������� ������, ���������� �� C++
		ScriptExports::Register();
		ScriptProcess::RegisterScriptClass();

		m_pGameLogic->Init();

		return true;
	}

	void GameApp::Close()
	{
		//������������ ���� ��������
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