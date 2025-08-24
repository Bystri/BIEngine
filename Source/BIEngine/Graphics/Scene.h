#pragma once

#include <memory>
#include <stack>

#include "Skybox.h"
#include "Camera.h"
#include "RenderItemsStorage.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ConstantsBuffer.h"
#include "../StdLib/UniquePtr.h"
#include "../EventManager/Events.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class GraphicsRenderPass;

// Владеет всеми графическими элементами, необходимым для отрисовки
class Scene {
public:
   explicit Scene(SharedPtr<Renderer> pRenderer);

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

   int OnPreRender(const GameTimer& gt);
   int OnPostRender(const GameTimer& gt);

   void SetCamera(SharedPtr<Camera> pCamera) { m_pCamera = pCamera; }

   const SharedPtr<Camera> GetCamera() const { return m_pCamera; }

   RenderItemsStorage* GetRenderItemsStorage() const { return m_pRenderItemsStorage.Get(); }

   void AddRenderPass(SharedPtr<GraphicsRenderPass> pRenderPass) { m_graphicsRenderPasses.PushBack(pRenderPass); }

   SharedPtr<Renderer> GetRenderer() { return m_pRenderer; }

private:
   struct GlobalRenderBufferData {
      glm::mat4 projMat;
      glm::mat4 viewMat;
      glm::vec3 viewPos;
      float totalTime;
   };

   GlobalRenderBufferData m_globalRenderBufferData;

protected:
   SharedPtr<Camera> m_pCamera;

   // Рисовальщик
   SharedPtr<Renderer> m_pRenderer;
   SharedPtr<ConstantsBuffer> m_pConstantsBuffer;

   DynamicArray<SharedPtr<GraphicsRenderPass>> m_graphicsRenderPasses;
   UniquePtr<RenderItemsStorage> m_pRenderItemsStorage;
};

} // namespace BIEngine
