#pragma once

#include <vector>
#include <map>

#include <LuaPlus/LuaPlus.h>

#include "ActorComponent.h"


namespace BIEngine
{

	//Данный компонент отвечает за предоставление интерефейса для добавления актера в среду выполнения Lua
	//и определяет функции, которые могут быть вызваны из Lua-скриптов
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

		//Функции компонента для скрипта
		LuaPlus::LuaObject GetActorId();

		//Функции физики для скрипта
		LuaPlus::LuaObject GetPos() const;
		void SetPos(LuaPlus::LuaObject newPos);
		LuaPlus::LuaObject GetVelocity() const;
		float GetOrientation();
		float GetRotation();
		//TODO: добавить остальные функции для управления физическими свойствами актера

	private:
		std::string m_constructorName;
		std::string m_destructorName;

		//Здесь мы храним данные скрипта, чтобы добавить из в генерируемый XML при необходимости
		std::map<std::string, std::string> m_scriptData;

		LuaPlus::LuaObject m_scriptObject;
		LuaPlus::LuaObject m_scriptConstructor;
		LuaPlus::LuaObject m_scriptDestructor;
	};

	static ActorComponent* CreateScriptComponent()
	{
		return new ScriptComponent;
	}
}
