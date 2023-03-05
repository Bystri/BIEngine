#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "GameLogic.h"

namespace BIEngine 
{

	struct Options
	{
		//Указывает, использовать ли хранилище ресурсов как обычную папку, или нет
		//То есть, если значение true - будет использована обычная папка
		//Если значение false - архив
		bool useDevelopmentAssets = false;

		//Имя мира, который загрузится первым
		std::string mainWorldResName = "Worlds/World.xml";

		unsigned int screenWidth = 720;
		unsigned int screenHeight = 540;
	};


	//Данный класс инициализирует все системы игрового движка и отвечает за взаимодейтсвие движка и ОС 
	class GameApp
	{
	public:
		explicit GameApp(std::shared_ptr<GameLogic> pGameLogic);
		virtual ~GameApp();

		GameApp(const GameApp& orig) = delete;
		GameApp& operator=(const GameApp& rhs) = delete;

		virtual bool Init();
		virtual void Close();

		std::shared_ptr<HumanView> TryGetHumanView(unsigned int playerId);

		virtual const char* GetGameTitle() = 0; //Принудительный метод заставить пользователя дать явное название игре

		void ProcessInput(double dt);
		void OnUpdate(double dt);
		void OnRender(double time, double dt);

		//Обработчик событий устройств ввода
		void InputProc(int key, int scancode, bool pressed);

	public:
		const std::shared_ptr<GameLogic> m_pGameLogic;

		Options m_options;
	};

	extern GameApp* g_pApp;
}