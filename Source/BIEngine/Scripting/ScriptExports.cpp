#include "ScriptExports.h"

#include <iostream>
#include <set>

#include <LuaPlus/LuaPlus.h>

#include "../EngineCore/GameApp.h"
#include "LuaStateManager.h"
#include "ScriptEvent.h"
#include "../Actors/Actor.h"
#include "../ProcessManager/ProcessManager.h"
#include "../ResourceCache/ResCache.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{
	//Слушатель событий внутри C++ кода, который будет вызывать функцию внутри lua, при возникновении события нужного типа
	class ScriptEventListener
	{
	public:
		explicit ScriptEventListener(const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction);
		~ScriptEventListener();
		EventListenerDelegate GetDelegate() { return fastdelegate::MakeDelegate(this, &ScriptEventListener::ScriptEventDelegate); }
		void ScriptEventDelegate(IEventDataPtr pEventPtr);

	private:
		EventType m_eventType;
		LuaPlus::LuaObject m_scriptCallbackFunction;
	};

	//Знает обо всех слушателях событий внутри скрипта
	class ScriptEventListenerMgr
	{
		typedef std::set<ScriptEventListener*> ScriptEventListenerSet;

	public:
		~ScriptEventListenerMgr();
		void AddListener(ScriptEventListener* pListener);
		void DestroyListener(ScriptEventListener* pListener);

	private:
		ScriptEventListenerSet m_listeners;
	};

	//Данный класс содержит в себе функции, которые являются обертками над системами движка, доступными из скрипта
	class InternalScriptExports
	{
	public:
		static bool Init();
		static void Destroy();

		static bool LoadAndExecuteScriptResource(const char* pScriptResource);

		static int CreateActor(const char* actorArchetype);

		static void AttachScriptProcess(LuaPlus::LuaObject scriptProcess);

		static unsigned long RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction);
		static void RemoveEventListener(unsigned long listenerId);
		static bool QueueEvent(EventType eventType, LuaPlus::LuaObject eventData);
		static bool TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData);

		static void SetPosition(LuaPlus::LuaObject positionLua, int actorId);
		static void SetVelocity(LuaPlus::LuaObject velocityLua, int actorId);
		static void StopActor(int actorId);
		static void ApplyForce(LuaPlus::LuaObject normalDirLua, float force, int actorId);
		static void ApplyTorque(LuaPlus::LuaObject axisLua, float force, int actorId);

	private:
		static std::shared_ptr<ScriptEvent> BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData);

	private:
		static ScriptEventListenerMgr* s_pScriptEventListenerMgr;
	};

	ScriptEventListenerMgr* InternalScriptExports::s_pScriptEventListenerMgr = nullptr;

	void ScriptExports::Register()
	{
		LuaPlus::LuaObject globals = LuaStateManager::Get()->GetGlobalVars();

		InternalScriptExports::Init();

		//ЗАГРУЗКА РЕСУРСОВ:
		globals.RegisterDirect("LoadAndExecuteScriptResource", &InternalScriptExports::LoadAndExecuteScriptResource);

		//АКТЕРЫ
		globals.RegisterDirect("CreateActor", &InternalScriptExports::CreateActor);

		//СИСТЕМА ПРОЦЕССОВ
		globals.RegisterDirect("AttachProcess", &InternalScriptExports::AttachScriptProcess);

		//СИСТЕМА СОБЫТИЙ
		globals.RegisterDirect("RegisterEventListener", &InternalScriptExports::RegisterEventListener);
		globals.RegisterDirect("RemoveEventListener", &InternalScriptExports::RemoveEventListener);
		globals.RegisterDirect("QueueEvent", &InternalScriptExports::QueueEvent);
		globals.RegisterDirect("TriggerEvent", &InternalScriptExports::TriggerEvent);

		//ФИЗИКА
		globals.RegisterDirect("SetPosition", &InternalScriptExports::SetPosition);
		globals.RegisterDirect("SetVelocity", &InternalScriptExports::SetVelocity);
		globals.RegisterDirect("StopActor", &InternalScriptExports::StopActor);
		globals.RegisterDirect("ApplyForce", &InternalScriptExports::ApplyForce);
		globals.RegisterDirect("ApplyTorque", &InternalScriptExports::ApplyTorque);
	}

	void ScriptExports::Unregister()
	{
		InternalScriptExports::Destroy();
	}


	bool InternalScriptExports::Init()
	{
		assert(s_pScriptEventListenerMgr == nullptr);
		s_pScriptEventListenerMgr = new ScriptEventListenerMgr;

		return true;
	}

	void InternalScriptExports::Destroy()
	{
		if (s_pScriptEventListenerMgr)
		{
			delete s_pScriptEventListenerMgr;
			s_pScriptEventListenerMgr = nullptr;
		}
	}

	bool InternalScriptExports::LoadAndExecuteScriptResource(const char* scriptResource)
	{
		//Само выполнение скрипта происходит прямо внутри системы кэширования во время его загрузки
		std::shared_ptr<ResHandle> pResourceHandle = ResCache::Get()->GetHandle(scriptResource);
		if (pResourceHandle)
		{
			Logger::WriteLog(Logger::LogType::MESSAGE, "Script " + std::string(scriptResource) + " got successful!");
			return true;
		}

		return false;
	}

	int InternalScriptExports::CreateActor(const char* actorArchetype)
	{
		std::shared_ptr<BIEngine::XmlExtraData> pActorData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle(actorArchetype)->GetExtra());
		std::shared_ptr<Actor> pActor = g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement());

		if (pActor)
		{
			//std::shared_ptr<EvtData_New_Actor> pNewActorEvent(GCC_NEW EvtData_New_Actor(pActor->GetId()));
			//IEventManager::Get()->VQueueEvent(pNewActorEvent);
			return pActor->GetId();
		}

		return Actor::INVALID_ACTOR_ID;
	}

	void InternalScriptExports::AttachScriptProcess(LuaPlus::LuaObject scriptProcess)
	{
		LuaPlus::LuaObject temp = scriptProcess.Lookup("__object");
		if (!temp.IsNil())
		{
			std::shared_ptr<Process> pProcess(static_cast<Process*>(temp.GetLightUserData()));
			ProcessManager::Get()->AttachProcess(pProcess);
		}
		else
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Couldn't find __object in script process");
		}
	}


	unsigned long InternalScriptExports::RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction)
	{
		assert(s_pScriptEventListenerMgr);

		if (callbackFunction.IsFunction())
		{
			//Создаем C++ прокси-слушателя, который ссылает на настоящую функцию-слушателя внутри скрипта
			ScriptEventListener* pListener = new ScriptEventListener(eventType, callbackFunction);
			s_pScriptEventListenerMgr->AddListener(pListener);
			EventManager::Get()->AddListener(pListener->GetDelegate(), eventType);

			unsigned long handle = reinterpret_cast<unsigned long>(pListener);
			return handle;
		}

		Logger::WriteLog(Logger::LogType::ERROR, "Attempting to register script event listener with invalid callback function");
		return 0;
	}


	void InternalScriptExports::RemoveEventListener(unsigned long listenerId)
	{
		assert(s_pScriptEventListenerMgr);
		assert(listenerId != 0);

		ScriptEventListener* pListener = reinterpret_cast<ScriptEventListener*>(listenerId);
		s_pScriptEventListenerMgr->DestroyListener(pListener);
	}

	bool InternalScriptExports::QueueEvent(EventType eventType, LuaPlus::LuaObject eventData)
	{
		std::shared_ptr<ScriptEvent> pEvent(BuildEvent(eventType, eventData));
		if (pEvent)
		{
			EventManager::Get()->QueueEvent(pEvent);
			return true;
		}
		return false;
	}

	bool InternalScriptExports::TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData)
	{
		std::shared_ptr<ScriptEvent> pEvent(BuildEvent(eventType, eventData));
		if (pEvent)
			return EventManager::Get()->TriggerEvent(pEvent);
		return false;
	}

	std::shared_ptr<ScriptEvent> InternalScriptExports::BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData)
	{
		std::shared_ptr<ScriptEvent> pEvent(ScriptEvent::CreateEventFromScript(eventType));
		if (!pEvent)
			return std::shared_ptr<ScriptEvent>();

		if (!pEvent->SetEventData(eventData))
		{
			return std::shared_ptr<ScriptEvent>();
		}

		return pEvent;
	}

	void InternalScriptExports::SetPosition(LuaPlus::LuaObject velocityLua, int actorId)
	{
		if (velocityLua.IsTable())
		{
			glm::vec2 velocity(velocityLua["x"].GetFloat(), velocityLua["y"].GetFloat());
			g_pApp->m_pGameLogic->GetGamePhysics()->SetPosition(actorId, velocity);
			return;
		}

		Logger::WriteLog(Logger::LogType::ERROR, "Invalid object passed to SetPosition(); type = " + std::string(velocityLua.TypeName()));
	}

	void InternalScriptExports::SetVelocity(LuaPlus::LuaObject velocityLua, int actorId)
	{
		if (velocityLua.IsTable())
		{
			glm::vec2 velocity(velocityLua["x"].GetFloat(), velocityLua["y"].GetFloat());
			g_pApp->m_pGameLogic->GetGamePhysics()->SetVelocity(actorId, velocity);
			return;
		}

		Logger::WriteLog(Logger::LogType::ERROR, "Invalid object passed to SetVelocity(); type = " + std::string(velocityLua.TypeName()));
	}

	void InternalScriptExports::StopActor(int actorId)
	{
		g_pApp->m_pGameLogic->GetGamePhysics()->StopActor(actorId);
	}

	void InternalScriptExports::ApplyForce(LuaPlus::LuaObject normalDirLua, float force, int actorId)
	{
		if (normalDirLua.IsTable())
		{
			glm::vec2 normalDir(normalDirLua["x"].GetFloat(), normalDirLua["y"].GetFloat());
			g_pApp->m_pGameLogic->GetGamePhysics()->ApplyForce(normalDir, force, actorId);
			return;
		}

		Logger::WriteLog(Logger::LogType::ERROR, "Invalid object passed to ApplyForce(); type = " + std::string(normalDirLua.TypeName()));
	}

	void InternalScriptExports::ApplyTorque(LuaPlus::LuaObject axisLua, float force, int actorId)
	{
		if (axisLua.IsTable())
		{
			glm::vec2 axis(axisLua["x"].GetFloat(), axisLua["y"].GetFloat());
			g_pApp->m_pGameLogic->GetGamePhysics()->ApplyTorque(axis, force, actorId);
			return;
		}
		
		Logger::WriteLog(Logger::LogType::ERROR, "Invalid object passed to ApplyTorque(); type = " + std::string(axisLua.TypeName()));
	}


	ScriptEventListenerMgr::~ScriptEventListenerMgr()
	{
		for (auto it = m_listeners.begin(); it != m_listeners.end(); ++it)
		{
			ScriptEventListener* pListener = (*it);
			delete pListener;
		}
		m_listeners.clear();
	}

	void ScriptEventListenerMgr::AddListener(ScriptEventListener* pListener)
	{
		m_listeners.insert(pListener);
	}

	void ScriptEventListenerMgr::DestroyListener(ScriptEventListener* pListener)
	{
		ScriptEventListenerSet::iterator findIt = m_listeners.find(pListener);
		if (findIt != m_listeners.end())
		{
			m_listeners.erase(findIt);
			delete pListener;
		}
		else
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Couldn't find script listener in set; this will probably cause a memory leak");
		}
	}



	ScriptEventListener::ScriptEventListener(const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction)
		: m_scriptCallbackFunction(scriptCallbackFunction)
	{
		m_eventType = eventType;
	}

	ScriptEventListener::~ScriptEventListener()
	{
		EventManager::Get()->RemoveListener(GetDelegate(), m_eventType);
	}

	//Вызов настоящей функции-слушателя внутри скрипта
	void ScriptEventListener::ScriptEventDelegate(IEventDataPtr pEvent)
	{
		assert(m_scriptCallbackFunction.IsFunction()); 

		//Вызов lua-функции
		std::shared_ptr<ScriptEvent> pScriptEvent = std::static_pointer_cast<ScriptEvent>(pEvent);
		LuaPlus::LuaFunction<void> Callback = m_scriptCallbackFunction;
		Callback(pScriptEvent->GetEventData());
	}

}