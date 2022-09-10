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

//������ ������ ������ ���� �������� � ����������� ������ ��� ������� ������������ �� ScriptEvent ������� ��� ����������� ����������� ������ REGISTER_SCRIPT_EVENT
#define EXPORT_FOR_SCRIPT_EVENT(eventClass) \
	public: \
		static ScriptEvent* CreateEventForScript() \
		{ \
			return new eventClass; \
		}

	//������ ����� ������������ ��� �������� ��� ���� �������, ������� ������ ���������� �� C++ � Lua � �������
	//���� ����, ����� ������� ��� �� C++ � Lua: ��������� �������������� ������� BuildEventData
	//���� ����, ����� ������� ��� �� Lua � C++: ��������� �������������� ������� BuildEventFromScript
	//���� ����, ����� ������� ��� � ��� �������, ��������� �������������� ��� ��� �������
	class ScriptEvent : public BaseEventData
	{
		typedef std::map<EventType, CreateEventForScriptFunctionType> CreationFunctions;

	public:
		ScriptEvent() { m_eventDataIsValid = false; }

		LuaPlus::LuaObject GetEventData();
		bool SetEventData(LuaPlus::LuaObject eventData);

		//������������ ��� ������� � ���������� ������� ������ Lua
		static void RegisterEventTypeWithScript(const char* key, EventType type);
		static void AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr);
		static ScriptEvent* CreateEventFromScript(EventType type);

	protected:
		//����� ������ ���� ��������� ��������, ������� ����� ������� � �������, ����������� ������ Lua-�������
		virtual void BuildEventData();
		//����� �������� ���� ��������� ��������, ������� ����� ������� � �������, ���������� �� Lua-�������
		virtual bool BuildEventFromScript() { return true; }

	protected:
		LuaPlus::LuaObject m_eventData;

	private:
		static CreationFunctions s_creationFunctions;
		bool m_eventDataIsValid;
	};

}
