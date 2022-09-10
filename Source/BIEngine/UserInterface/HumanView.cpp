#include "HumanView.h"

#include "../Graphics2D/Opengl/ShadersLoaderOpenGL.h"
#include "../Graphics2D/Shader.h"
#include "../Graphics2D/Opengl/TextureLoaderOpenGL.h"
#include "../Graphics2D/Opengl/SpriteNode.h"
#include "../EngineCore/GameApp.h"
#include "../Audio/irrKlangAudio.h"

namespace BIEngine
{

	const unsigned int SCREEN_REFRESH_RATE = 1000 / 60;

	HumanView::HumanView(unsigned int screenWidth, unsigned int screenHeight)
		: m_viewId(INVALID_GAME_VIEW_ID)

		, m_userInterface()
		, m_pRenderer(nullptr)
		, m_pCameraActor(nullptr)
		, m_scene(nullptr)

		, m_currTick(0.0f)
		, m_lastDraw(0.0f)
		, m_isRunFullSpeed(true)

		, m_pointerRadius(1.0f)
		, m_pKeyboardHandler()

		, m_screenWidth(screenWidth)
		, m_screenHeight(screenHeight)
	{
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &HumanView::NewCameraComponentDelegate), EvtData_New_Camera_Component::sk_EventType);
	}

	HumanView::~HumanView()
	{

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

		//Матрица камеры
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight), 0.0f, -1.0f, 1.0f);

		//Загрузка шейдеров
		std::shared_ptr<OpenglShaderData> pVertShaderData = std::static_pointer_cast<OpenglShaderData>(ResCache::Get()->GetHandle("Effects/sprite.vs")->GetExtra());
		std::shared_ptr<OpenglShaderData> pFragShaderxData = std::static_pointer_cast<OpenglShaderData>(ResCache::Get()->GetHandle("Effects/sprite.frag")->GetExtra());
		Shader shader;
		shader.Compile(pVertShaderData->GetShaderIndex(), pFragShaderxData->GetShaderIndex());
		shader.Use();
		shader.SetInteger("image", 0);
		shader.SetMatrix4("projection", projection);

		//Создание отображения
		m_pRenderer = std::make_shared<Renderer>(shader);
		//Создания сцены на основе отоьбражения
		m_scene = new Scene(m_pRenderer);

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

	void HumanView::OnUpdate(float dt)
	{

	}

	void HumanView::OnRender(float fTime, float dt)
	{
		m_scene->OnRender();
		m_userInterface.OnRender(dt);
	}

	void HumanView::SetKey(int key, int scancode, bool state)
	{
		if (m_pKeyboardHandler)
			if (state)
				m_pKeyboardHandler->OnKeyDown(key, scancode);
			else
				m_pKeyboardHandler->OnKeyUp(key, scancode);
	}

	//Вызывается, когда создается новый актер с компонентой-камерой.
	void HumanView::NewCameraComponentDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_New_Camera_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Camera_Component>(pEventData);

		m_pCameraActor = pCastEventData->GetCameraActor();
		
	}
}