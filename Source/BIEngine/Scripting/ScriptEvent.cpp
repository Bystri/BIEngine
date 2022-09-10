#include "ScriptEvent.h"

#include <cassert>
#include <iostream>

#include "LuaStateManager.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{

	ScriptEvent::CreationFunctions ScriptEvent::s_creationFunctions;

	LuaPlus::LuaObject ScriptEvent::GetEventData()
	{
		if (!m_eventDataIsValid)
		{
			BuildEventData();
			m_eventDataIsValid = true;
		}

		return m_eventData;
	}

	bool ScriptEvent::SetEventData(LuaPlus::LuaObject eventData)
	{
		m_eventData = eventData;
		m_eventDataIsValid = BuildEventFromScript();
		return m_eventDataIsValid;
	}

	void ScriptEvent::RegisterEventTypeWithScript(const char* key, EventType type)
	{
		LuaPlus::LuaObject eventTypeTable = LuaStateManager::Get()->GetGlobalVars().GetByName("EventType");
		if (eventTypeTable.IsNil())
			eventTypeTable = LuaStateManager::Get()->GetGlobalVars().CreateTable("EventType");

		assert(eventTypeTable.IsTable());
		assert(eventTypeTable[key].IsNil());

		eventTypeTable.SetInteger(key, type);
	}

	void ScriptEvent::AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr)
	{
		assert(s_creationFunctions.find(type) == s_creationFunctions.end());
		assert(pCreationFunctionPtr != NULL);
		s_creationFunctions.insert(std::make_pair(type, pCreationFunctionPtr));
	}

	ScriptEvent* ScriptEvent::CreateEventFromScript(EventType type)
	{
		CreationFunctions::iterator findIt = s_creationFunctions.find(type);
		if (findIt != s_creationFunctions.end())
		{
			CreateEventForScriptFunctionType func = findIt->second;
			return func();
		}
		else
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Couldn't find event type");
			return NULL;
		}
	}

	void ScriptEvent::BuildEventData()
	{
		//Поведение поумолчанию для BuildEventData задает приравнивает данные события к нулю.
		m_eventData.AssignNil(LuaStateManager::Get()->GetLuaState());
	}

}