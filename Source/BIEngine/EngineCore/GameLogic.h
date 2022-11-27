#pragma once

#include "../Actors/ActorFactory.h"
#include "../UserInterface/HumanView.h"
#include "../Physics/Physics.h"

namespace BIEngine
{

	class GameLogic
	{
	public:
		typedef std::map<ActorId, std::shared_ptr<Actor>> ActorMap;

		GameLogic();
		virtual ~GameLogic();

		GameLogic(const GameLogic& orig) = delete;
		GameLogic& operator=(const GameLogic& rhs) = delete;

		std::shared_ptr<IGamePhysics> GetGamePhysics() const { return m_pPhysics; }

		virtual bool Init();

		bool LoadLevel(const std::string& path);

		//Должен быть переопределен дочерним классом, если нужно сделать что-то особое во время загрузки мира.
		//Такая необохдимость в отдельной функции возникает из-за выполнение скриптов перед и после загрузки мира.
		virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) { return true; }

		std::shared_ptr<Actor> CreateActor(tinyxml2::XMLElement* pRoot);
		//Принимает на вход XML-структуру актера, компоненты в котором будут заменены или добавлены.
		void ModifyActor(ActorId actorId, tinyxml2::XMLElement* pOverrides);
		//Является ответчиком на запрос об уничтожении актера
		void RequestDestroyActorDelegate(IEventDataPtr pEventData);
		virtual void DestroyActor(const ActorId actorId);

		virtual void AddGameView(std::shared_ptr<IGameView> pView);
		virtual void RemoveGameView(std::shared_ptr<IGameView> pView);

		virtual void OnUpdate(float dt);
		virtual void OnRender(float time, float dt);

		int GetNumActors() const { return m_actors.size(); };

		//Обработчики событий устройств ввода
		void SetKey(int key, int scancode, bool state);

	protected:
		GameViewList m_gameViews;

		ActorMap m_actors;
		ActorFactory* m_pActorFactory;

		std::shared_ptr<IGamePhysics> m_pPhysics;
	};

}