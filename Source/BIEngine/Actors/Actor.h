#pragma once

#include <tinyxml2.h>

#include "ActorComponent.h"
#include "../StdLib/String.h"
#include "../StdLib/DynamicArray.h"
#include "../StdLib/SharedPtr.h"
#include "../StdLib/HashMap.h"

namespace BIEngine {

using ActorId = unsigned long;

// Объект, который является главным дейтсвующим механизмом на сцене. Основные его свойства задаются с помощью компонентов, которые описываются в XML-файле актера
class Actor {
   friend class ActorFactory;

   using ActorComponents = HashMap<ComponentId, std::shared_ptr<ActorComponent>>;

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
   std::weak_ptr<ComponentType> GetComponent(ComponentId id)
   {
      auto findIt = m_components.Find(id);
      if (findIt != m_components.End()) {
         std::shared_ptr<ActorComponent> pBase(findIt->second);

         // Приведение к подклассу типа компонента
         std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
         std::weak_ptr<ComponentType> pWeakSub(pSub); // Приведение сильного указателя к слабому
         return pWeakSub;
      }

      return std::weak_ptr<ComponentType>();
   }

private:
   // Так как создание/инициализация/модификация актера идет через ActorFactory, то следующие методы доступны только ему.

   explicit Actor(ActorId id);

   // Инициализация информации об актера на основе настроек в XML файле
   // Не отвечает за создание или инициализацию компонентов
   bool Init(tinyxml2::XMLElement* pData);

   // Данный метод может вызываться только фабрикой
   void AddComponent(std::shared_ptr<ActorComponent> pComponent);

private:
   // Идентификатор актера, по которому к нему идет обращение из других систем
   ActorId m_id;
   bool m_bIsActivated;
   bool m_activateFlag;
   bool m_isLevelLoaded;

   // Поле несет чисто косметический характер. Используется в редакторе.
   String m_name;
   ActorComponents m_components;

   Actor* m_pParent;
   DynamicArray<SharedPtr<Actor>> m_children;
};

} // namespace BIEngine
