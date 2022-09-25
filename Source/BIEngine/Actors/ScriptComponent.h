#pragma once

#include <vector>

#include <LuaPlus/LuaPlus.h>

#include "ActorComponent.h"


namespace BIEngine
{

	//������ ��������� �������� �� �������������� ����������� ��� ���������� ������ � ����� ���������� Lua
	//� ���������� �������, ������� ����� ���� ������� �� Lua-��������
	class ScriptComponent : public ActorComponent
	{
	public:
		static ComponentId g_CompId;
		virtual ComponentId GetComponentId() const { return ScriptComponent::g_CompId; }

		ScriptComponent();
		virtual ~ScriptComponent();
		virtual bool Init(tinyxml2::XMLElement* pData);
		virtual void PostInit() override;
		virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

		static void RegisterScriptFunctions();
		static void UnregisterScriptFunctions();

	private:
		void CreateScriptObject();

		//������� ���������� ��� �������
		LuaPlus::LuaObject GetActorId();

		//������� ������ ��� �������
		LuaPlus::LuaObject GetPos();
		void SetPos(LuaPlus::LuaObject newPos);
		//TODO: �������� ��������� ������� ��� ���������� ����������� ���������� ������

	private:
		std::string m_constructorName;
		std::string m_destructorName;

		//����� �� ������ ������ �������, ����� �������� �� � ������������ XML ��� �������������
		std::vector<std::pair<std::string, std::string>> m_scriptData;

		LuaPlus::LuaObject m_scriptObject;
		LuaPlus::LuaObject m_scriptConstructor;
		LuaPlus::LuaObject m_scriptDestructor;
	};

	static ActorComponent* CreateScriptComponent()
	{
		return new ScriptComponent;
	}
}
