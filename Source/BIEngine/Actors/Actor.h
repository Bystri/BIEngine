#pragma once

#include <tinyxml2.h>

#include "ActorComponent.h"
#include "../StdLib/String.h"
#include "../StdLib/Bitset.h"
#include "../StdLib/DynamicArray.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/HashMap.h"

namespace BIEngine {

using ActorId = unsigned long;

// Объект, который является главным дейтсвующим механизмом на сцене. Основные его свойства задаются с помощью компонентов, которые описываются в XML-файле актера
class Actor {
   friend class ActorFactory;

   using ActorComponents = HashMap<ComponentId, SharedPtr<ActorComponent>>;

public:
   const static ActorId INVALID_ACTOR_ID = -1;

   ~Actor();

   /*
   Актеры хранят компоненты в виде умных указателей, поэтому стандартные конструкторы присванивания и копирования для них неприемлемо.
   Но, так как за создание полноценного актера отвечает фабрика, которая контролирует этапы создания и инициализации актера и его компонентов,
   сам актер не имеет возможности сделать свою полноценную копию.
   */
   Actor(const Actor& orig) = delete;
   Actor& operator=(const Actor& orig) = delete;

   Actor(Actor&& orig) = default;
   Actor& operator=(Actor&& orig) = default;

   void AddChild(SharedPtr<Actor> pChild);
   bool RemoveChild(ActorId id);

   void Activate();
   void Deactivate();

   void SetActivate(bool value);

   void OnLevelLoaded();

   void OnUpdate(const GameTimer& gt);
   void OnRenderObject(const GameTimer& gt);

   // Удаление всех умных указателей на компоненты текущего актера.
   void Destroy();

   // Генерация XML-файла, по которому можно будет создать копию актера с помощью объекта ActorFactory
   tinyxml2::XMLElement* ToXML(tinyxml2::XMLDocument* pDoc) const;

   // Идентификатор актера, по которому к нему идет обращение из других систем
   ActorId GetId() const { return m_id; }

   Actor* GetParent() const { return m_pParent; }

   // Поле несет чисто косметический характер. Используется в редакторе для распознования объектов
   const String& GetName() const { return m_name; }

   const DynamicArray<SharedPtr<Actor>>& GetChildren() const { return m_children; }

   Actor* GetActorByPath(const String& path);

   // Шаблон функции для получения компонентов
   template <class ComponentType>
   WeakPtr<ComponentType> GetComponent(ComponentId id)
   {
      auto findIt = m_components.Find(id);
      if (findIt != m_components.End()) {
         SharedPtr<ActorComponent> pBase(findIt->second);

         // Приведение к подклассу типа компонента
         SharedPtr<ComponentType> pSub(StaticPointerCast<ComponentType>(pBase));
         WeakPtr<ComponentType> pWeakSub(pSub); // Приведение сильного указателя к слабому
         return pWeakSub;
      }

      return WeakPtr<ComponentType>();
   }

private:
   // Так как создание/инициализация/модификация актера идет через ActorFactory, то следующие методы доступны только ему.

   explicit Actor(ActorId id);

   // Инициализация информации об актера на основе настроек в XML файле
   // Не отвечает за создание или инициализацию компонентов
   bool Init(tinyxml2::XMLElement* pData);

   // Данный метод может вызываться только фабрикой
   void AddComponent(SharedPtr<ActorComponent> pComponent);

private:
   static constexpr int IS_ACTIVATED_FLAG_IDX = 0;
   static constexpr int IS_ACTIVE_FLAG_IDX = 1;
   static constexpr int IS_LEVEL_LOADED_FLAG_IDX = 2;

   // Поле несет чисто косметический характер. Используется в редакторе.
   String m_name;
   ActorComponents m_components;

   Actor* m_pParent;
   DynamicArray<SharedPtr<Actor>> m_children;

   // Идентификатор актера, по которому к нему идет обращение из других систем
   ActorId m_id;
   Bitset<32> m_flags;
};

} // namespace BIEngine
