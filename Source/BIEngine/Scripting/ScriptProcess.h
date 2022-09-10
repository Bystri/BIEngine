#pragma once

#include <LuaPlus/LuaPlus.h>

#include "../ProcessManager/Process.h"
#include "LuaStateManager.h"

namespace BIEngine
{

	class ScriptProcess : public Process
	{

	public:
		static void RegisterScriptClass();

	protected:
		virtual void OnInit();
		virtual void OnUpdate(unsigned long deltaMs);
		virtual void OnSuccess();
		virtual void OnFail();
		virtual void OnAbort();

	private:
		//Привязывает основные функции для работы с процессов к функциям мета-таблицы
		static void RegisterScriptClassFunctions(LuaPlus::LuaObject& metaTableObj);
		static LuaPlus::LuaObject CreateFromScript(LuaPlus::LuaObject self,
			LuaPlus::LuaObject constructionData,
			LuaPlus::LuaObject originalSubClass);

		//Инициализирует ScriptProcess-объект в cpp-коде на основе заранее подготовленного "класса" из lua-скрипта
		virtual bool BuildCppDataFromScript(LuaPlus::LuaObject scriptClass,
			LuaPlus::LuaObject constructionData);

		bool ScriptIsAlive() { return IsAlive(); }
		bool ScriptIsDead() { return IsDead(); }
		bool ScriptIsPaused() { return IsPaused(); }

		void ScriptAttachChild(LuaPlus::LuaObject child);

		explicit ScriptProcess();

	private:
		unsigned long m_frequency, m_time;
		LuaPlus::LuaObject m_scriptInitFunction, m_scriptUpdateFunction;
		LuaPlus::LuaObject m_scriptSuccessFunction, m_scriptFailFunction;
		LuaPlus::LuaObject m_scriptAbortFunction;
		LuaPlus::LuaObject m_self;
	};

}