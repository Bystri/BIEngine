#include "ScriptProcess.h"

#include <iostream>

#include "../Utilities/Logger.h"

namespace BIEngine
{

	const char* SCRIPT_PROCESS_NAME = "ScriptProcess";

	ScriptProcess::ScriptProcess()
	{
		LuaPlus::LuaState* pLuaState = LuaStateManager::Get()->GetLuaState();

		m_frequency = 0;
		m_time = 0;

		//Приравниваем все функции к нулю для данного экземпляра процесса
		m_scriptInitFunction.AssignNil(pLuaState);
		m_scriptUpdateFunction.AssignNil(pLuaState);
		m_scriptSuccessFunction.AssignNil(pLuaState);
		m_scriptFailFunction.AssignNil(pLuaState);
		m_scriptAbortFunction.AssignNil(pLuaState);
	}

	void ScriptProcess::RegisterScriptClass()
	{
		//Создаем мета таблицу внутри lua, которая будет использоваться для создания lua-процессов
		LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(SCRIPT_PROCESS_NAME);
		metaTableObj.SetObject("__index", metaTableObj);
		metaTableObj.SetObject("base", metaTableObj);
		metaTableObj.SetBoolean("cpp", true);
		RegisterScriptClassFunctions(metaTableObj);
		metaTableObj.RegisterDirect("Create", &ScriptProcess::CreateFromScript);
	}

	void ScriptProcess::OnInit()
	{
		Process::OnInit();
		if (!m_scriptInitFunction.IsNil())
		{
			LuaPlus::LuaFunction<void> func(m_scriptInitFunction);
			func(m_self);
		}

		if (!m_scriptUpdateFunction.IsFunction())
			Fail();
	}

	void ScriptProcess::OnUpdate(unsigned long deltaMs)
	{
		m_time += deltaMs;
		if (m_time >= m_frequency)
		{
			LuaPlus::LuaFunction<void> func(m_scriptUpdateFunction);
			func(m_self, m_time);
			m_time = 0;
		}
	}

	void ScriptProcess::OnSuccess()
	{
		if (!m_scriptSuccessFunction.IsNil())
		{
			LuaPlus::LuaFunction<void> func(m_scriptSuccessFunction);
			func(m_self);
		}
	}

	void ScriptProcess::OnFail()
	{
		if (!m_scriptFailFunction.IsNil())
		{
			LuaPlus::LuaFunction<void> func(m_scriptFailFunction);
			func(m_self);
		}
	}

	void ScriptProcess::OnAbort()
	{
		if (!m_scriptAbortFunction.IsNil())
		{
			LuaPlus::LuaFunction<void> func(m_scriptAbortFunction);
			func(m_self);
		}
	}

	bool ScriptProcess::BuildCppDataFromScript(LuaPlus::LuaObject scriptClass, LuaPlus::LuaObject constructionData)
	{
		if (scriptClass.IsTable())
		{
			// OnInit()
			LuaPlus::LuaObject temp = scriptClass.GetByName("OnInit");
			if (temp.IsFunction())
				m_scriptInitFunction = temp;

			// OnUpdate()
			temp = scriptClass.GetByName("OnUpdate");
			if (temp.IsFunction())
			{
				m_scriptUpdateFunction = temp;
			}
			else
			{
				Logger::WriteLog(Logger::LogType::ERROR, "No OnUpdate() found in script process; type == " + std::string(temp.TypeName()));
				return false;
			}

			// OnSuccess()
			temp = scriptClass.GetByName("OnSuccess");
			if (temp.IsFunction())
				m_scriptSuccessFunction = temp;

			// OnFail()
			temp = scriptClass.GetByName("OnFail");
			if (temp.IsFunction())
				m_scriptFailFunction = temp;

			// OnAbort()
			temp = scriptClass.GetByName("OnAbort");
			if (temp.IsFunction())
				m_scriptAbortFunction = temp;
		}
		else
		{
			Logger::WriteLog(Logger::LogType::ERROR, "scriptClass is not a table in ScriptProcess::BuildCppDataFromScript()");
			return false;
		}

		if (constructionData.IsTable())
		{

			for (LuaPlus::LuaTableIterator constructionDataIt(constructionData); constructionDataIt; constructionDataIt.Next())
			{
				const char* key = constructionDataIt.GetKey().GetString();
				LuaPlus::LuaObject val = constructionDataIt.GetValue();

				if (strcmp(key, "frequency") == 0 && val.IsInteger())
					m_frequency = val.GetInteger();
				else
					m_self.SetObject(key, val);
			}
		}

		return true;
	}


	LuaPlus::LuaObject ScriptProcess::CreateFromScript(LuaPlus::LuaObject self, LuaPlus::LuaObject constructionData, LuaPlus::LuaObject originalSubClass)
	{
		ScriptProcess* pObj = new ScriptProcess();

		pObj->m_self.AssignNewTable(LuaStateManager::Get()->GetLuaState());
		if (pObj->BuildCppDataFromScript(originalSubClass, constructionData))
		{
			LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(SCRIPT_PROCESS_NAME);
			assert(!metaTableObj.IsNil());

			pObj->m_self.SetLightUserData("__object", pObj);
			pObj->m_self.SetMetaTable(metaTableObj);
		}
		else
		{
			pObj->m_self.AssignNil(LuaStateManager::Get()->GetLuaState());
			delete pObj;
		}

		return pObj->m_self;
	}

	void ScriptProcess::RegisterScriptClassFunctions(LuaPlus::LuaObject& metaTableObj)
	{
		metaTableObj.RegisterObjectDirect("Succeed", (Process*)0, &Process::Succeed);
		metaTableObj.RegisterObjectDirect("Fail", (Process*)0, &Process::Fail);
		metaTableObj.RegisterObjectDirect("Pause", (Process*)0, &Process::Pause);
		metaTableObj.RegisterObjectDirect("UnPause", (Process*)0, &Process::UnPause);
		metaTableObj.RegisterObjectDirect("IsAlive", (ScriptProcess*)0, &ScriptProcess::ScriptIsAlive);
		metaTableObj.RegisterObjectDirect("IsDead", (ScriptProcess*)0, &ScriptProcess::ScriptIsDead);
		metaTableObj.RegisterObjectDirect("IsPaused", (ScriptProcess*)0, &ScriptProcess::ScriptIsPaused);
		metaTableObj.RegisterObjectDirect("AttachChild", (ScriptProcess*)0, &ScriptProcess::ScriptAttachChild);
	}

	void ScriptProcess::ScriptAttachChild(LuaPlus::LuaObject child)
	{
		if (child.IsTable())
		{
			LuaPlus::LuaObject obj = child.GetByName("__object");
			if (!obj.IsNil())
			{
				std::shared_ptr<Process> pProcess(static_cast<Process*>(obj.GetLightUserData()));
				assert(pProcess);
				AttachChild(pProcess);
			}
			else
			{
				Logger::WriteLog(Logger::LogType::ERROR, "Attempting to attach child to ScriptProcess with no valid __object");
			}
		}
		else
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Invalid object type passed into ScriptProcess::ScriptAttachChild(); type = " + std::string(child.TypeName()));
		}
	}

}