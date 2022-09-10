#pragma once

#include <list>
#include <memory>

enum class GameViewType
{
	Human,
	Remote, //В процессе
	AI, //В процессе
	Recorder, //В процессе
	Other
};

typedef unsigned int GameViewId;

class IGameView
{
public:
	static const GameViewId INVALID_GAME_VIEW_ID = 0xffffffff;

	virtual ~IGameView() { };

	virtual bool Init() = 0;

	virtual void OnUpdate(float dt) = 0;
	virtual void OnRender(float time, float dt) = 0;

	virtual GameViewType GetType() const = 0;
	virtual GameViewId GetId() const = 0;

	virtual void SetKey(int key, int scancode, bool state) = 0;
};

typedef std::list<std::shared_ptr<IGameView>> GameViewList;