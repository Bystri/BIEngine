#pragma once

#include <list>
#include <memory>

#include "../Utilities/GameTimer.h"

namespace BIEngine {

enum class GameViewType {
   Human,
   Remote,   // В процессе
   AI,       // В процессе
   Recorder, // В процессе
   Other
};

using GameViewId = unsigned int;

class IGameView {
public:
   static const GameViewId INVALID_GAME_VIEW_ID = 0xffffffff;

   virtual ~IGameView(){};

   virtual bool Init() = 0;

   virtual void OnPreRender(const GameTimer& gt) = 0;
   virtual void OnPostRender(const GameTimer& gt) = 0;

   virtual GameViewType GetType() const = 0;
   virtual GameViewId GetId() const = 0;

   virtual void OnPointerMove(float xpos, float ypos) = 0;
   virtual void SetPointerButton(int button, bool state) = 0;
   virtual void SetKey(int key, int scancode, bool state) = 0;
};

using GameViewList = std::list<std::shared_ptr<IGameView>>;

} // namespace BIEngine
