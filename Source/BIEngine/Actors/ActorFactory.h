#pragma once

#include <glm/glm.hpp>

#include "../StdLib/String.h"
#include "../StdLib/UniquePtr.h"
#include "../StdLib/HashMap.h"
#include "Actor.h"

namespace BIEngine {

using ActorComponentCreator = UniquePtr<ActorComponent> (*)();
using ActorComponentCreatorMap = HashMap<String, ActorComponentCreator>;

class ActorFactory {
public:
   ActorFactory();

   // Так как фабрика отвечает за создание и назначение актерам уникального идентификатора, по которому все системы будут ссылаться на данного актера,
   // копировать фабрики запрещено, по причине того, что тогда две фабрики будут выдавать актерам одинаковые ID.
   ActorFactory(const ActorFactory& orig) = delete;
   // Так как фабрика отвечает за создание и назначение актерам уникального идентификатора, по которому все системы будут ссылаться на данного актера,
   // копировать фабрики запрещено, по причине того, что тогда две фабрики будут выдавать актерам одинаковые ID.
   ActorFactory& operator=(const ActorFactory& orig) = delete;

   void AddComponentCreator(const String& name, ActorComponentCreator pComponentCreator)
   {
      m_actorComponentCreators.Insert(name, pComponentCreator);
   }

   // Создает актера со всеми требуемыми компонентами на основе XML-элемента.
   // Возвращает нулевой указатель, если требуется.
   SharedPtr<Actor> CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition = nullptr, const glm::vec3* const pRotation = nullptr, Actor* const pParent = nullptr);

   // Модифицирует актера на основе предоставленного XML-элемента
   // В предоставленном элементе может быть полное или частичное описание уже имеющегося компонента актера - тогда он просто будет обновлен
   // Также можно передовать полноценное описание нового компонента, такой компонент будет корректно создан, проинициализирован и добавлен актеру
   void ModifyActor(SharedPtr<Actor> pActor, tinyxml2::XMLElement* pOverrides);

protected:
   SharedPtr<Actor> CreateActorFromRootElement(tinyxml2::XMLElement* pRoot, Actor* const pParent = nullptr);

   // Создает компонент на основе информации из XML-структуры. Вернет нулевой указатель, если данный компонент не может быть создан
   virtual std::shared_ptr<ActorComponent> CreateComponent(SharedPtr<Actor> pActor, tinyxml2::XMLElement* pData);

private:
   ActorId GetNextActorId()
   {
      ++m_lastActorId;
      return m_lastActorId;
   }

protected:
   ActorComponentCreatorMap m_actorComponentCreators;

private:
   ActorId m_lastActorId;
};
} // namespace BIEngine
