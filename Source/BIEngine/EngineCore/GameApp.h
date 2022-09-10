#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "GameLogic.h"

namespace BIEngine 
{

	struct Options
	{
		//���������, ������������ �� ��������� �������� ��� ������� �����, ��� ���
		//�� ����, ���� �������� true - ����� ������������ ������� �����
		//���� �������� false - �����
		bool useDevelopmentAssets = false;

		unsigned int screenWidth = 720;
		unsigned int screenHeight = 540;
	};


	//������ ����� �������������� ��� ������� �������� ������ � �������� �� �������������� ������ � �� 
	class GameApp
	{
	public:
		explicit GameApp(std::shared_ptr<GameLogic> pGameLogic);
		virtual ~GameApp();

		GameApp(const GameApp& orig) = delete;
		GameApp& operator=(const GameApp& rhs) = delete;

		virtual bool Init();
		virtual void Close();

		virtual const char* GetGameTitle() = 0; //�������������� ����� ��������� ������������ ���� ����� �������� ����

		void ProcessInput(float dt);
		void OnUpdate(float dt);
		void OnRender(float time, float dt);

		//���������� ������� ��������� �����
		void InputProc(int key, int scancode, bool pressed);

	public:
		const std::shared_ptr<GameLogic> m_pGameLogic;

		Options m_options;
	};

	extern GameApp* g_pApp;
}