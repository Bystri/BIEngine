#pragma once

#include "IGameView.h"
#include "InputDevices.h"
#include "UserInterface.h"
#include "../Renderer/Renderer.h"
#include "../Graphics/Scene.h"

namespace BIEngine
{

	class HumanView : public IGameView
	{
	public:
		HumanView(unsigned int screenWidth, unsigned int screenHeight);
		virtual ~HumanView();			

		//Аудио
		//bool InitAudio();
		virtual bool Init();
		virtual void Shutdown();

		std::shared_ptr<Renderer> GetRenderer() const { return m_pRenderer; }

		virtual void OnUpdate(float dt) override;
		virtual void OnRender(float fTime, float dt) override;

		virtual GameViewType GetType() const override { return GameViewType::Human; }
		virtual GameViewId GetId() const override { return m_viewId; }

		virtual void OnPointerMove(float xpos, float ypos) override;
		virtual void SetKey(int key, int scancode, bool state) override;

	protected:
		GameViewId m_viewId;

		unsigned int m_screenWidth;
		unsigned int m_screenHeight;

		//Графическая часть
		UserInterface m_userInterface;
		std::shared_ptr<Renderer> m_pRenderer;
		Scene* m_pScene;	

		//Переменные для контроля скорости обновления кадров
		float m_currTick; //Текущее время отображения
		float m_lastDraw; //Последнее время, когда был отображен кадр
		bool m_isRunFullSpeed;

		//Устройства ввода
		std::shared_ptr<IPointerHandler> m_pPointerHandler;
		int m_pointerRadius;
		std::shared_ptr<IKeyboardHandler> m_pKeyboardHandler;
	};
}


