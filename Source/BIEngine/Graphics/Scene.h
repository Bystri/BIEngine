#pragma once

#include <map>
#include <memory>
#include <stack>

#include "SceneNodes.h"
#include "LightNodes.h"
#include "Skybox.h"
#include "Camera.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ConstantsBuffer.h"
#include "../EventManager/Events.h"

namespace BIEngine {

typedef std::map<ActorId, std::shared_ptr<ISceneNode>> SceneActorMap;

// Владеет всеми графическими элементами, необходимым для отрисовки
class Scene {
public:
   explicit Scene(std::shared_ptr<Renderer> pRenderer);
   virtual ~Scene();

   /*
   Так как сцены работают с указателями на актеров и их компоненты, мы не можем просто так копировать сцену (и актеров), так как сцена не единственные (а еще и не главный),
   держатель списка актеров. Тут они будут скопированы, а остальные системы об этом знать не будут
   TODO: в теории копирование актера регестрирует новый экземпляр во всех системах, но это еще не протестировано. Если возникнет НЕОБХОДИМОСТЬ в копировании сцен - можно будет
   заняться реализацией контроля копирования
   */
   Scene(const Scene& orig) = delete;
   Scene& operator=(const Scene& orig) = delete;
   Scene(Scene&& orig) = delete;
   Scene& operator=(Scene&& orig) = delete;

   void Init();

   int OnRender();
   int OnUpdate(float dt);

   std::shared_ptr<ISceneNode> FindActor(ActorId id);

   void SetCamera(std::shared_ptr<Camera> pCamera) { m_pCamera = pCamera; }

   const std::shared_ptr<Camera> GetCamera() const { return m_pCamera; }

   void SetSkybox(std::shared_ptr<Skybox> pSkybox) { m_pSkybox = pSkybox; }

   void AddChild(ActorId id, std::shared_ptr<ISceneNode> pChild);
   void RemoveChild(ActorId id);

   std::shared_ptr<Renderer> GetRenderer() { return m_pRenderer; }

protected:
   // Регистрация компонента, который необохдимо отрисовать
   void NewRenderComponentDelegate(IEventDataPtr pEventData);
   // Удаление уничтоженного актера из сцены
   void DestroyActorDelegate(IEventDataPtr pEventData);

private:
   struct GlobalRenderBufferData {
      glm::mat4 projMat;
      glm::mat4 viewMat;
   };

   GlobalRenderBufferData m_globalRenderBufferData;

protected:
   // Основной узел дерева графических элементов
   std::shared_ptr<SceneNode> m_pRoot;

   std::shared_ptr<Camera> m_pCamera;
   std::shared_ptr<Skybox> m_pSkybox;
   // Рисовальщик
   std::shared_ptr<Renderer> m_pRenderer;
   std::shared_ptr<ConstantsBuffer> m_pConstantsBuffer;

   SceneActorMap m_actorMap;
};

} // namespace BIEngine