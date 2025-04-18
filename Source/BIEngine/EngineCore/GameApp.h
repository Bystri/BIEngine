﻿#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "GameLogic.h"

namespace BIEngine {

struct Options {
   std::string playerName;

   // Указывает, использовать ли хранилище ресурсов как обычную папку, или нет
   // То есть, если значение true - будет использована обычная папка
   // Если значение false - архив
   bool useDevelopmentAssets = false;
   bool isEditorMode = false;

   // Имя мира, который загрузится первым
   std::string mainWorldResNamePath = "Worlds/World";

   std::string hostAddress = "";
   uint16_t hostPort = 0;

   unsigned int screenWidth = 1024;
   unsigned int screenHeight = 768;
   bool maximazeWindow = false;
};

// Данный класс инициализирует все системы игрового движка и отвечает за взаимодейтсвие движка и ОС
class GameApp {
public:
   explicit GameApp(std::shared_ptr<GameLogic> pGameLogic);
   virtual ~GameApp();

   GameApp(const GameApp& orig) = delete;
   GameApp& operator=(const GameApp& rhs) = delete;

   virtual bool Init();
   virtual void Close();

   std::shared_ptr<HumanView> TryGetHumanView(unsigned int playerId);

   virtual const char* GetGameTitle() = 0; // Принудительный метод заставить пользователя дать явное название игре

   void ProcessInput(const GameTimer& gt);
   void OnUpdate(GameTimer& gt);
   void OnRender(const GameTimer& gt);

   // Обработчик событий устройств ввода
   void OnPointerMove(float xpos, float ypos);
   void SetPointerButton(int button, bool state);
   void SetKey(int key, int scancode, bool pressed);

public:
   const std::shared_ptr<GameLogic> m_pGameLogic;

   Options m_options;
};

extern GameApp* g_pApp;
} // namespace BIEngine
