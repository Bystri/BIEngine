#pragma once

#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameApp.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{

	//�������� ������ ��� GLFW
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	//������� �������, � ������� ���������� ������ ����� ����������.
	//����� �� ������� ������ ���� ��������������� ����� pGameApp
	int Run(int argc, char* argv[])
	{
		if (!g_pApp)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "g_pApp must be initialized");
			return -1;
		}

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_RESIZABLE, false);

		GLFWwindow* window = glfwCreateWindow(g_pApp->m_options.screenWidth, g_pApp->m_options.screenHeight, g_pApp->GetGameTitle(), nullptr, nullptr);
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize GLAD");
			return -1;
		}

		glfwSetKeyCallback(window, key_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		glViewport(0, 0, g_pApp->m_options.screenWidth, g_pApp->m_options.screenHeight);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//������������� ����
		if (!g_pApp->Init())
		{
			//�� �� ����� ������ � ���, ��� ��� ��������������, ��� ������������� ������� ����� ��������� ��������� �� ������
			return -1;
		}

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;
		//�������� ����
		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwPollEvents();

			g_pApp->ProcessInput(deltaTime);
			//���������� ������
			g_pApp->OnUpdate(deltaTime);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			//���������
			g_pApp->OnRender(currentFrame, deltaTime);

			glfwSwapBuffers(window);
		}

		g_pApp->Close();

		glfwTerminate();
		return 0;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		//������� �� ����������, ���� ������ ������� Esc
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		
		if (action == GLFW_PRESS)
			g_pApp->InputProc(key, scancode, true);
		else
			g_pApp->InputProc(key, scancode, false);
	}

	//���������� Viewport ��� ����� ������� ������
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

}