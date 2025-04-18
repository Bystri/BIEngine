﻿#pragma once

#include <map>
#include <memory>
#include <stack>

#include "Skybox.h"
#include "Camera.h"
#include "RenderItemsStorage.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ConstantsBuffer.h"
#include "../EventManager/Events.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class GraphicsRenderPass;

// Владеет всеми графическими элементами, необходимым для отрисовки
class Scene {
public:
   explicit Scene(std::shared_ptr<Renderer> pRenderer);

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

   void SetCamera(std::shared_ptr<Camera> pCamera) { m_pCamera = pCamera; }

   const std::shared_ptr<Camera> GetCamera() const { return m_pCamera; }

   RenderItemsStorage* GetRenderItemsStorage() const { return m_pRenderItemsStorage.get(); }

   void AddRenderPass(std::shared_ptr<GraphicsRenderPass> pRenderPass) { m_graphicsRenderPasses.push_back(pRenderPass); }

   std::shared_ptr<Renderer> GetRenderer() { return m_pRenderer; }

private:
   struct GlobalRenderBufferData {
      glm::mat4 projMat;
      glm::mat4 viewMat;
      glm::vec3 viewPos;
      float totalTime;
   };

   GlobalRenderBufferData m_globalRenderBufferData;

protected:
   std::shared_ptr<Camera> m_pCamera;

   // Рисовальщик
   std::shared_ptr<Renderer> m_pRenderer;
   std::shared_ptr<ConstantsBuffer> m_pConstantsBuffer;

   std::vector<std::shared_ptr<GraphicsRenderPass>> m_graphicsRenderPasses;
   std::unique_ptr<RenderItemsStorage> m_pRenderItemsStorage;
};

} // namespace BIEngine
