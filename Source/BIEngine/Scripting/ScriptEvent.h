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

//ƒанный макрос должен быть вставлен в определение класса дл€ каждого производного от ScriptEvent событи€ дл€ дальнейшего корректного вызова REGISTER_SCRIPT_EVENT
#define EXPORT_FOR_SCRIPT_EVENT(eventClass) \
	public: \
		static ScriptEvent* CreateEventForScript() \
		{ \
			return new eventClass; \
		}

	//ƒанный класс предназначен как родитель дл€ всех событий, которые должны переходить от C++ к Lua и обратно
	//≈сли надо, чтобы событие шло от C++ к Lua: требутес€ переопределить функцию BuildEventData
	//≈сли надо, чтобы событие шло от Lua к C++: требутес€ переопределить функцию BuildEventFromScript
	//≈сли надо, чтобы событие шло в обе стороны, требуетс€ переопределить обу эти функции
	class ScriptEvent : public BaseEventData
	{
		typedef std::map<EventType, CreateEventForScriptFunctionType> CreationFunctions;

	public:
		ScriptEvent() { m_eventDataIsValid = false; }

		LuaPlus::LuaObject GetEventData();
		bool SetEventData(LuaPlus::LuaObject eventData);

		//–егестрирует тип событи€ в глобальной таблице внутри Lua
		static void RegisterEventTypeWithScript(const char* key, EventType type);
		static void AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr);
		static ScriptEvent* CreateEventFromScript(EventType type);

	protected:
		//«десь должны быть определны дейтсви€, которые будут сделаны с данными, попадающими внутрь Lua-скрипта
		virtual void BuildEventData();
		//«десь должныть быть определны дейтсви€, которые будут сделаны с данными, пришедшими из Lua-скрипта
		virtual bool BuildEventFromScript() { return true; }

	protected:
		LuaPlus::LuaObject m_eventData;

	private:
		static CreationFunctions s_creationFunctions;
		bool m_eventDataIsValid;
	};

}
