#pragma once

#include <string>
#include <cassert>

#include <LuaPlus/LuaPlus.h>
#include <glm/glm.hpp>

namespace BIEngine
{

	class LuaStateManager
	{
	public:
		//Инициализация экземпляра одиночки 
		static bool Create();
		//Уничтожение экземпляра одиночки
		static void Destroy();
		static LuaStateManager* Get() { assert(s_pSingleton); return s_pSingleton; }

		bool Init();
		//Выполняет указанный lua-скрипт
		//Может быть вызван внутри самого скрипта
		void ExecuteFile(const char* resource);
		//Выполняет указанный lua-код
		//Может быть вызван внутри самого скрипта
		void ExecuteString(const char* str);

		LuaPlus::LuaObject GetGlobalVars();
		LuaPlus::LuaState* GetLuaState() const;

		void ConvertVec2ToTable(const glm::vec2& vec, LuaPlus::LuaObject& outLuaTable) const;
		void ConvertTableToVec2(const LuaPlus::LuaObject& luaTable, glm::vec2& outVec) const;

	private:
		void SetError(int errorNum);
		void ClearStack();

		//Конструктуры приватны из-за того, что LuaStateManager реализован с помощью паттерна "одиночка"
		explicit LuaStateManager();
		~LuaStateManager();

	private:
		static LuaStateManager* s_pSingleton;
		LuaPlus::LuaState* m_pLuaState;
		std::string m_lastError;
	};

}