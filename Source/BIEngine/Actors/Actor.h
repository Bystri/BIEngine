#pragma once

#include <map>
#include <memory>
#include <string>

#include <tinyxml2.h>

#include "ActorComponent.h"

namespace BIEngine {

typedef unsigned long ActorId;

// Объект, который является главным дейтсвующим механизмом на сцене. Основные его свойства задаются с помощью компонентов, которые описываются в XML-файле актера
class Actor {
   friend class ActorFactory;

   typedef std::map<ComponentId, std::shared_ptr<ActorComponent>> ActorComponents;

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

   // Удаление всех умных указателей на компоненты текущего актера.
   void Destroy();

   // Генерация XML-файла, по которому можно будет создать копию актера с помощью объекта ActorFactory
   tinyxml2::XMLElement* Actor::ToXML(tinyxml2::XMLDocument* pDoc) const;

   // Идентификатор актера, по которому к нему идет обращение из других систем
   ActorId GetId() const { return m_id; }

   // Поле несет чисто косметический характер. Используется в редакторе для распознования объектов
   std::string GetName() const { return m_name; }

   // Шаблон функции для получения компонентов
   template <class ComponentType>
   std::weak_ptr<ComponentType> GetComponent(ComponentId id)
   {
      ActorComponents::iterator findIt = m_components.find(id);
      if (findIt != m_components.end()) {
         std::shared_ptr<ActorComponent> pBase(findIt->second);

         // Приведение к подклассу типа компонента
         std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
         std::weak_ptr<ComponentType> pWeakSub(pSub); // Приведение сильного указателя к слабому
         return pWeakSub;
      } else {
         return std::weak_ptr<ComponentType>();
      }
   }

private:
   // Так как создание/инициализация/модификация актера идет через ActorFactory, то следующие методы доступны только ему.

   explicit Actor(ActorId id);

   // Инициализация информации об актера на основе настроек в XML файле
   // Не отвечает за создание или инициализацию компонентов
   bool Init(tinyxml2::XMLElement* pData);
   // Вызывается после инициализации актера и его компонентов. Вызывает PostInit() для каждого компонента актера
   void PostInit();

   // Данный метод может вызываться только фабрикой
   void AddComponent(std::shared_ptr<ActorComponent> pComponent);

private:
   // Идентификатор актера, по которому к нему идет обращение из других систем
   ActorId m_id;
   // Поле несет чисто косметический характер. Используется в редакторе.
   std::string m_name;
   ActorComponents m_components;
};

} // namespace BIEngine
