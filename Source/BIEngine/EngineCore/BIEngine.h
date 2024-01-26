#pragma once

#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "GameApp.h"
#include "../Utilities/GameTimer.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

// Обратные вызовы для GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);

void InitImgui(GLFWwindow* window);
void TerminateImgui();

// Главная функция, с которой начинается работа всего приложения.
// Перед ее вызовом должен быть инициализирован класс pGameApp
int Run(int argc, char* argv[])
{
   if (!g_pApp) {
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

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      Logger::WriteLog(Logger::LogType::ERROR, "Failed to initialize GLAD");
      return -1;
   }

   glfwSetKeyCallback(window, KeyCallback);
   glfwSetCursorPosCallback(window, MouseCallback);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   glViewport(0, 0, g_pApp->m_options.screenWidth, g_pApp->m_options.screenHeight);

   InitImgui(window);

   // Инициализация игры
   if (!g_pApp->Init()) {
      // Мы не пишем ничего в лог, так как предпологается, что инициализация оставит более подробное сообщение об ошибке
      return -1;
   }

   GameTimer gt;
   gt.Start();
   // Основной цикл
   while (!glfwWindowShouldClose(window)) {
      gt.Tick();

      glfwPollEvents();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      g_pApp->ProcessInput(gt);
      // Обновление логики
      g_pApp->OnUpdate(gt);

      // Отрисовка
      g_pApp->OnRender(gt);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
   }

   g_pApp->Close();

   TerminateImgui();

   glfwTerminate();
   return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
   // Выходим из приложения, если нажата клавиша Esc
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
   }

   g_pApp->InputProc(key, scancode, action);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
   g_pApp->OnPointerMove(static_cast<float>(xpos), static_cast<float>(ypos));
}

// Адпатируем Viewport под новые размеры экрана
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}

void InitImgui(GLFWwindow* window)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
   ImGui_ImplOpenGL3_Init();
}

void TerminateImgui()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

} // namespace BIEngine
