#pragma once

#include <LuaPlus/LuaPlus.h>

#include "../EventManager/EventManager.h"



namespace BIEngine
{

	class ScriptEvent;
	typedef ScriptEvent* (*CreateEventForScriptFunctionType)();


#define REGISTER_SCRIPT_EVENT(eventClass, eventType) \
	ScriptEvent::RegisterEventTypeWithScript(#eventClass, eventType); \
	ScriptEvent::AddCreationFunction(eventType, &eventClass::CreateEventForScript)

//Данный макрос должен быть вставлен в определение класса для каждого производного от ScriptEvent события для дальнейшего корректного вызова REGISTER_SCRIPT_EVENT
#define EXPORT_FOR_SCRIPT_EVENT(eventClass) \
	public: \
		static ScriptEvent* CreateEventForScript() \
		{ \
			return new eventClass; \
		}

	//Данный класс предназначен как родитель для всех событий, которые должны переходить от C++ к Lua и обратно
	//Если надо, чтобы событие шло от C++ к Lua: требутеся переопределить функцию BuildEventData
	//Если надо, чтобы событие шло от Lua к C++: требутеся переопределить функцию BuildEventFromScript
	//Если надо, чтобы событие шло в обе стороны, требуется переопределить обу эти функции
	class ScriptEvent : public BaseEventData
	{
		typedef std::map<EventType, CreateEventForScriptFunctionType> CreationFunctions;

	public:
		ScriptEvent() { m_eventDataIsValid = false; }

		LuaPlus::LuaObject GetEventData();
		bool SetEventData(LuaPlus::LuaObject eventData);

		//Регестрирует тип события в глобальной таблице внутри Lua
		static void RegisterEventTypeWithScript(const char* key, EventType type);
		static void AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr);
		static ScriptEvent* CreateEventFromScript(EventType type);

	protected:
		//Здесь должны быть определны дейтсвия, которые будут сделаны с данными, попадающими внутрь Lua-скрипта
		virtual void BuildEventData();
		//Здесь должныть быть определны дейтсвия, которые будут сделаны с данными, пришедшими из Lua-скрипта
		virtual bool BuildEventFromScript() { return true; }

	protected:
		LuaPlus::LuaObject m_eventData;

	private:
		static CreationFunctions s_creationFunctions;
		bool m_eventDataIsValid;
	};

}
