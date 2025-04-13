#pragma once

#include "../Actors/ActorFactory.h"
#include "../UserInterface/HumanView.h"
#include "../Physics/Physics2D.h"
#include "../Physics/Physics3D.h"
#include "../Navigation/NavWorld.h"

namespace BIEngine {

class GameLogic {
   friend class GameApp;

public:
   using ActorMap = std::map<ActorId, std::shared_ptr<Actor>>;

   GameLogic();
   virtual ~GameLogic();

   GameLogic(const GameLogic& orig) = delete;
   GameLogic& operator=(const GameLogic& rhs) = delete;

   std::shared_ptr<IGamePhysics2D> GetGamePhysics2D() const { return m_pPhysics2D; }

   std::shared_ptr<IGamePhysics3D> GetGamePhysics3D() const { return m_pPhysics3D; }

   std::unique_ptr<NavWorld>& GetNavWorld() { return m_pNavWorld; }

   virtual bool Init();

   virtual void Terminate() {}

   bool LoadLevel(const std::string& path);

   bool IsLevelLoaded() const { return m_bIsLevelLoaded; };

   std::shared_ptr<Actor> CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition = nullptr, const glm::vec3* const pRotation = nullptr);
   // Принимает на вход XML-структуру актера, компоненты в котором будут заменены или добавлены.
   void ModifyActor(ActorId actorId, tinyxml2::XMLElement* pOverrides);
   // Является ответчиком на запрос об уничтожении актера
   void RequestDestroyActorDelegate(IEventDataPtr pEventData);
   virtual void DestroyActor(const ActorId actorId);

   virtual void AddGameView(std::shared_ptr<IGameView> pView);
   virtual void RemoveGameView(std::shared_ptr<IGameView> pView);

   virtual void OnUpdate(GameTimer& gt);
   virtual void OnRender(const GameTimer& gt);

   std::shared_ptr<Actor> GetActor(ActorId id) const;

   int GetNumActors() const { return m_actors.size(); };

   const ActorMap& GetActors() const { return m_actors; }

   // Обработчики событий устройств ввода
   void OnPointerMove(float xpos, float ypos);
   void SetPointerButton(int button, bool state);
   void SetKey(int key, int scancode, bool state);

protected:
   // Должен быть переопределен дочерним классом, если нужно сделать что-то особое во время загрузки мира.
   // Такая необохдимость в отдельной функции возникает из-за выполнение скриптов перед и после загрузки мира.
   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) { return true; }

protected:
   bool m_bIsLevelLoaded;

   GameViewList m_gameViews;

   ActorMap m_actors;
   ActorFactory* m_pActorFactory;

   std::shared_ptr<IGamePhysics2D> m_pPhysics2D;
   std::shared_ptr<IGamePhysics3D> m_pPhysics3D;
   std::unique_ptr<NavWorld> m_pNavWorld;
};

} // namespace BIEngine
